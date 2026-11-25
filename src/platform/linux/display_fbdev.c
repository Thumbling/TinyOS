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

static void flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    int32_t width = area->x2 - area->x1 + 1;
    int32_t height = area->y2 - area->y1 + 1;

    if (width <= 0 || height <= 0) {
        lv_display_flush_ready(display);
        return;
    }

    for (int32_t row = 0; row < height; row++) {
        int32_t y = area->y1 + row;
        if (y < 0 || y >= (int32_t)fb_var.yres)
            continue;

        size_t destination_offset =
            (size_t)(y + (int32_t)fb_var.yoffset) * fb_fix.line_length +
            (size_t)((area->x1 < 0 ? 0 : area->x1) + (int32_t)fb_var.xoffset) * (fb_var.bits_per_pixel / 8);
        const uint8_t *source = pixels + (size_t)row * (size_t)width * 2;

        if (area->x1 < 0 || area->x1 + width > (int32_t)fb_var.xres) {
            /* Partial clipping is not expected: LVGL renders inside the display. */
            continue;
        }

        if (fb_var.bits_per_pixel == 16) {
            memcpy(fb_memory + destination_offset, source, (size_t)width * 2);
        } else if (fb_var.bits_per_pixel == 32) {
            uint32_t *destination = (uint32_t *)(fb_memory + destination_offset);
            const uint16_t *input = (const uint16_t *)source;
            for (int32_t x = 0; x < width; x++) {
                uint16_t pixel = input[x];
                uint32_t r = (pixel >> 11) & 0x1f;
                uint32_t g = (pixel >> 5) & 0x3f;
                uint32_t b = pixel & 0x1f;
                uint32_t r8 = (r * 255 + 15) / 31;
                uint32_t g8 = (g * 255 + 31) / 63;
                uint32_t b8 = (b * 255 + 15) / 31;
                destination[x] = 0xff000000u | (r8 << 16) | (g8 << 8) | b8;
            }
        }
    }

    lv_display_flush_ready(display);
}

lv_display_t *tinyos_display_create(void)
{
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

    fb_memory = mmap(NULL, fb_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_memory == MAP_FAILED) {
        TINYOS_LOGE("mmap fb failed: %s", strerror(errno));
        fb_memory = NULL;
        close(fb_fd);
        fb_fd = -1;
        return NULL;
    }

    memset(fb_memory, 0, fb_memory_size);

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
