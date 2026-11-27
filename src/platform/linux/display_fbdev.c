/*
 * TinyOS fbdev display backend.
 *
 * The mainline sun8i-v3s RG Nano device tree enables panel-mipi-dbi-spi,
 * which exposes the panel through the DRM fbdev compatibility node /dev/fb0.
 * LVGL renders RGB565; this backend copies dirty rectangles into the mmap'd
 * framebuffer and converts to XRGB8888 when the kernel exposes a 32-bit fb.
 */
#include "../../platform.h"
#include "../../core/log.h"
#include "fbdev_blit.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define FB_DEVICE "/dev/fb0"
#define BUFFER_LINES 40
#define MAX_WIDTH 320

static int fb_fd = -1;
static uint8_t *fb_memory;
static size_t fb_memory_size;
static struct fb_fix_screeninfo fb_fix;
static struct fb_var_screeninfo fb_var;
static uint8_t draw_buffer_a[MAX_WIDTH * BUFFER_LINES * 2];
static uint8_t draw_buffer_b[MAX_WIDTH * BUFFER_LINES * 2];
static tinyos_fbdev_geometry fb_geometry;

static int text_has_ci(const char *text, const char *needle)
{
    size_t needle_length = strlen(needle);
    for (const char *start = text; *start; start++) {
        size_t i = 0;
        while (i < needle_length && start[i] &&
               tolower((unsigned char)start[i]) == tolower((unsigned char)needle[i]))
            i++;
        if (i == needle_length)
            return 1;
    }
    return 0;
}

/* A manual first start must not bypass the run-script handoff. This mirrors
 * the helper's fail-closed policy before the framebuffer is opened. */
static int fbcon_is_bound(void)
{
    DIR *directory = opendir("/sys/class/vtconsole");
    if (!directory) {
        TINYOS_LOGE("cannot inspect /sys/class/vtconsole: %s", strerror(errno));
        return 1;
    }

    int found_fbcon = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strncmp(entry->d_name, "vtcon", 5) != 0)
            continue;

        char path[512];
        snprintf(path, sizeof(path), "/sys/class/vtconsole/%s/name", entry->d_name);
        FILE *name_file = fopen(path, "r");
        if (!name_file) {
            closedir(directory);
            TINYOS_LOGE("cannot read %s; refusing to open framebuffer", path);
            return 1;
        }
        char name[128];
        if (!fgets(name, sizeof(name), name_file)) {
            fclose(name_file);
            closedir(directory);
            TINYOS_LOGE("cannot read %s; refusing to open framebuffer", path);
            return 1;
        }
        fclose(name_file);
        name[strcspn(name, "\r\n")] = '\0';

        if (text_has_ci(name, "dummy"))
            continue;

        if (!text_has_ci(name, "frame") && !text_has_ci(name, "fbcon") &&
            !text_has_ci(name, "framebuffer")) {
            closedir(directory);
            TINYOS_LOGE("unknown VT console '%s'; refusing to open framebuffer", name);
            return 1;
        }

        found_fbcon = 1;
        snprintf(path, sizeof(path), "/sys/class/vtconsole/%s/bind", entry->d_name);
        FILE *bind_file = fopen(path, "r");
        if (!bind_file) {
            closedir(directory);
            TINYOS_LOGE("cannot read %s; refusing to open framebuffer", path);
            return 1;
        }
        int state = -1;
        if (fscanf(bind_file, "%d", &state) != 1)
            state = -1;
        fclose(bind_file);
        if (state != 0) {
            closedir(directory);
            TINYOS_LOGE("%s is still bound (bind=%d); refusing to open framebuffer",
                        entry->d_name, state);
            return 1;
        }
    }
    closedir(directory);

    if (!found_fbcon && access("/sys/class/graphics/fbcon", F_OK) == 0) {
        TINYOS_LOGE("fbcon sysfs exists without a verified vtconsole; refusing to open framebuffer");
        return 1;
    }
    return 0;
}

static void flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    tinyos_fbdev_blit_rgb565(&fb_geometry, area->x1, area->y1, area->x2, area->y2,
                             pixels);
    lv_display_flush_ready(display);
}

lv_display_t *tinyos_display_create(void)
{
    if (fbcon_is_bound()) {
        TINYOS_LOGE("fbcon handoff not verified; refusing to open %s", FB_DEVICE);
        return NULL;
    }

    for (int attempt = 0; attempt < 50; attempt++) {
        fb_fd = open(FB_DEVICE, O_RDWR | O_CLOEXEC);
        if (fb_fd >= 0)
            break;
        if (errno != ENOENT && errno != EACCES)
            break;
        usleep(100000);
    }

    if (fb_fd < 0) {
        TINYOS_LOGE("cannot open %s: %s", FB_DEVICE, strerror(errno));
        return NULL;
    }

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_fix) != 0 ||
        ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_var) != 0) {
        TINYOS_LOGE("cannot query %s: %s", FB_DEVICE, strerror(errno));
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    if (fb_var.bits_per_pixel != 16 && fb_var.bits_per_pixel != 32) {
        TINYOS_LOGE("unsupported fb depth: %u bpp", fb_var.bits_per_pixel);
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    if (fb_var.xres == 0 || fb_var.yres == 0 || fb_fix.line_length == 0) {
        TINYOS_LOGE("invalid fb geometry");
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    if (fb_var.xres > MAX_WIDTH) {
        TINYOS_LOGW("fb width %u exceeds draw buffer (%d); display may glitch",
                    fb_var.xres, MAX_WIDTH);
    }

    fb_memory_size = fb_fix.smem_len;
    if (fb_memory_size == 0)
        fb_memory_size = (size_t)fb_fix.line_length * fb_var.yres_virtual;

    /* The clear must reach every visible row, not just the virtual screen
     * base. Reject a mapping whose stride or size cannot hold the panel. */
    size_t row_bytes = (size_t)(fb_var.xoffset + fb_var.xres) * (fb_var.bits_per_pixel / 8);
    size_t visible_bytes = (size_t)(fb_var.yoffset + fb_var.yres) * fb_fix.line_length;
    if (fb_fix.line_length < row_bytes || fb_memory_size < visible_bytes) {
        TINYOS_LOGE("fb mapping too small: %zu bytes, need %zu", fb_memory_size, visible_bytes);
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    fb_memory = mmap(NULL, fb_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_memory == MAP_FAILED) {
        TINYOS_LOGE("mmap fb failed: %s", strerror(errno));
        fb_memory = NULL;
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    fb_geometry = (tinyos_fbdev_geometry){
        .memory = fb_memory,
        .size = fb_memory_size,
        .stride = fb_fix.line_length,
        .width = fb_var.xres,
        .height = fb_var.yres,
        .xoffset = fb_var.xoffset,
        .yoffset = fb_var.yoffset,
        .bits_per_pixel = fb_var.bits_per_pixel,
    };
    if (!tinyos_fbdev_geometry_valid(&fb_geometry)) {
        TINYOS_LOGE("fb geometry cannot cover the visible panel");
        munmap(fb_memory, fb_memory_size);
        fb_memory = NULL;
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    /* Full mapped clear before LVGL creates its first object. Partial refresh
     * never guarantees a full-screen paint, so no fbcon image may be allowed
     * to survive in regions the first layout does not invalidate. */
    tinyos_fbdev_clear(&fb_geometry);
    TINYOS_LOGI("fb0 cleared %zu mapped bytes before first frame", fb_memory_size);

    lv_display_t *display = lv_display_create((int32_t)fb_var.xres, (int32_t)fb_var.yres);
    if (!display) {
        munmap(fb_memory, fb_memory_size);
        fb_memory = NULL;
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, draw_buffer_a, draw_buffer_b, sizeof(draw_buffer_a),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_cb);

    TINYOS_LOGI("fb0 %ux%u %u bpp, stride %u", fb_var.xres, fb_var.yres,
                fb_var.bits_per_pixel, fb_fix.line_length);
    return display;
}

void tinyos_game_present_rgb565(const uint16_t *pixels, int width, int height)
{
    if (!fb_memory || !pixels || width <= 0 || height <= 0)
        return;
    if (width > (int)fb_geometry.width)
        width = (int)fb_geometry.width;
    if (height > (int)fb_geometry.height)
        height = (int)fb_geometry.height;

    /* The emulator owns the whole panel; clear once so letterbox bars are
     * deterministic even if the panel was left with an older image. */
    tinyos_fbdev_clear(&fb_geometry);
    tinyos_fbdev_blit_rgb565(&fb_geometry, 0, 0, width - 1, height - 1,
                             (const uint8_t *)pixels);
}
