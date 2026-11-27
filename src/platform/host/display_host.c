#include "../../platform.h"

#include <stdint.h>
#include <string.h>

#define HOST_WIDTH 240
#define HOST_HEIGHT 240
#define HOST_BUFFER_LINES 40
#define STALE_A 0xDEADu
#define STALE_B 0xBEEFu

static bool stale_framebuffer_requested;

static uint16_t framebuffer[HOST_WIDTH * HOST_HEIGHT];
static uint8_t write_coverage[HOST_WIDTH * HOST_HEIGHT];
static uint8_t draw_buffer_a[HOST_WIDTH * HOST_BUFFER_LINES * 2];
static uint8_t draw_buffer_b[HOST_WIDTH * HOST_BUFFER_LINES * 2];

static void flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    int32_t width = area->x2 - area->x1 + 1;
    int32_t height = area->y2 - area->y1 + 1;
    const uint16_t *source = (const uint16_t *)pixels;

    for (int32_t y = 0; y < height; y++) {
        int32_t destination_y = area->y1 + y;
        if (destination_y < 0 || destination_y >= HOST_HEIGHT)
            continue;

        int32_t source_x = 0;
        int32_t destination_x = area->x1;
        int32_t copy_width = width;
        if (destination_x < 0) {
            source_x = -destination_x;
            copy_width += destination_x;
            destination_x = 0;
        }
        if (destination_x + copy_width > HOST_WIDTH)
            copy_width = HOST_WIDTH - destination_x;
        if (copy_width > 0) {
            memcpy(&framebuffer[destination_y * HOST_WIDTH + destination_x],
                   &source[source_x], (size_t)copy_width * sizeof(uint16_t));
            memset(&write_coverage[destination_y * HOST_WIDTH + destination_x], 1,
                   (size_t)copy_width);
        }
        source += width;
    }
    lv_display_flush_ready(display);
}

lv_display_t *tinyos_display_create(void)
{
    if (stale_framebuffer_requested) {
        /* Test-only: stand in for a console image left by fbcon before the
         * first GUI frame. The normal host path clears to black like fbdev. */
        for (size_t i = 0; i < sizeof(framebuffer) / sizeof(framebuffer[0]); i++)
            framebuffer[i] = (i & 1u) ? STALE_A : STALE_B;
    } else {
        memset(framebuffer, 0, sizeof(framebuffer));
    }

    lv_display_t *display = lv_display_create(HOST_WIDTH, HOST_HEIGHT);
    if (!display)
        return NULL;

    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, draw_buffer_a, draw_buffer_b, sizeof(draw_buffer_a),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_cb);
    return display;
}

const uint16_t *tinyos_host_framebuffer(void)
{
    return framebuffer;
}

void tinyos_host_set_stale_framebuffer(bool enabled)
{
    stale_framebuffer_requested = enabled;
    if (enabled) {
        for (size_t i = 0; i < sizeof(framebuffer) / sizeof(framebuffer[0]); i++)
            framebuffer[i] = (i & 1u) ? STALE_A : STALE_B;
        memset(write_coverage, 0, sizeof(write_coverage));
    }
}

uint32_t tinyos_host_framebuffer_stale_count(void)
{
    uint32_t stale = 0;
    for (size_t i = 0; i < sizeof(framebuffer) / sizeof(framebuffer[0]); i++) {
        if (framebuffer[i] == STALE_A || framebuffer[i] == STALE_B)
            stale++;
    }
    return stale;
}

uint32_t tinyos_host_framebuffer_written_count(void)
{
    uint32_t written = 0;
    for (size_t i = 0; i < sizeof(write_coverage) / sizeof(write_coverage[0]); i++) {
        if (write_coverage[i])
            written++;
    }
    return written;
}

void tinyos_game_present_rgb565(const uint16_t *pixels, int width, int height)
{
    if (!pixels || width <= 0 || height <= 0)
        return;
    if (width > HOST_WIDTH)
        width = HOST_WIDTH;
    if (height > HOST_HEIGHT)
        height = HOST_HEIGHT;

    for (int y = 0; y < HOST_HEIGHT; y++) {
        for (int x = 0; x < HOST_WIDTH; x++) {
            uint16_t value = 0;
            if (x < width && y < height)
                value = pixels[y * width + x];
            framebuffer[y * HOST_WIDTH + x] = value;
            write_coverage[y * HOST_WIDTH + x] = 1;
        }
    }
}
