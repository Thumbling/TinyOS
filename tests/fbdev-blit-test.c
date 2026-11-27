/* Exercise the exact fbdev clear/blit code with a fake panel mapping. */
#include "fbdev_blit.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PANEL_WIDTH 240
#define PANEL_HEIGHT 240
#define PANEL_PIXELS (PANEL_WIDTH * PANEL_HEIGHT)

static int fail(const char *what)
{
    printf("fbdev first frame: FAIL: %s\n", what);
    return 1;
}

int main(void)
{
    static uint8_t mapping[PANEL_WIDTH * PANEL_HEIGHT * 4];
    static uint8_t pixels[PANEL_WIDTH * PANEL_HEIGHT * 2];
    tinyos_fbdev_geometry geometry = {
        .memory = mapping,
        .size = sizeof(mapping),
        .stride = PANEL_WIDTH * 4,
        .width = PANEL_WIDTH,
        .height = PANEL_HEIGHT,
        .xoffset = 0,
        .yoffset = 0,
        .bits_per_pixel = 32,
    };

    if (!tinyos_fbdev_geometry_valid(&geometry))
        return fail("valid 32-bpp geometry rejected");

    tinyos_fbdev_geometry bad = geometry;
    bad.size = geometry.stride * PANEL_HEIGHT - 4;
    if (tinyos_fbdev_geometry_valid(&bad))
        return fail("undersized mapping accepted");
    bad = geometry;
    bad.bits_per_pixel = 24;
    if (tinyos_fbdev_geometry_valid(&bad))
        return fail("unsupported depth accepted");

    memset(mapping, 0x5a, sizeof(mapping));
    tinyos_fbdev_clear(&geometry);
    for (size_t i = 0; i < sizeof(mapping); i++) {
        if (mapping[i] != 0)
            return fail("full clear left a non-zero byte");
    }

    for (size_t i = 0; i < PANEL_PIXELS; i++) {
        uint16_t pixel = (uint16_t)((i * 2654435761u) >> 16);
        pixels[i * 2] = (uint8_t)(pixel & 0xff);
        pixels[i * 2 + 1] = (uint8_t)(pixel >> 8);
    }
    tinyos_fbdev_blit_rgb565(&geometry, 0, 0, PANEL_WIDTH - 1, PANEL_HEIGHT - 1,
                             pixels);

    for (size_t i = 0; i < PANEL_PIXELS; i++) {
        uint16_t pixel = (uint16_t)((i * 2654435761u) >> 16);
        uint32_t r = (pixel >> 11) & 0x1f;
        uint32_t g = (pixel >> 5) & 0x3f;
        uint32_t b = pixel & 0x1f;
        uint32_t expected = 0xff000000u |
                            (((r * 255 + 15) / 31) << 16) |
                            (((g * 255 + 31) / 63) << 8) |
                            ((b * 255 + 15) / 31);
        uint32_t actual;
        memcpy(&actual, mapping + i * 4, sizeof(actual));
        if (actual != expected)
            return fail("32-bpp blit did not cover every panel pixel");
    }

    static uint8_t rgb565[PANEL_WIDTH * PANEL_HEIGHT * 2];
    tinyos_fbdev_geometry rgb565_geometry = {
        .memory = rgb565,
        .size = sizeof(rgb565),
        .stride = PANEL_WIDTH * 2,
        .width = PANEL_WIDTH,
        .height = PANEL_HEIGHT,
        .bits_per_pixel = 16,
    };
    memset(rgb565, 0x5a, sizeof(rgb565));
    tinyos_fbdev_clear(&rgb565_geometry);
    tinyos_fbdev_blit_rgb565(&rgb565_geometry, 0, 0, PANEL_WIDTH - 1,
                             PANEL_HEIGHT - 1, pixels);
    if (memcmp(rgb565, pixels, sizeof(pixels)) != 0)
        return fail("16-bpp blit did not cover every panel pixel");

    printf("fbdev first frame: clear + 57600-pixel blit, 32/16 bpp: PASS\n");
    return 0;
}
