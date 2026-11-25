#include "png.h"

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static void rgb565_to_rgb888(const uint16_t *src, uint8_t *dst, int pixels)
{
    for (int i = 0; i < pixels; i++) {
        uint16_t p = src[i];
        uint8_t r = (uint8_t)(((p >> 11) & 0x1f) * 255 / 31);
        uint8_t g = (uint8_t)(((p >> 5) & 0x3f) * 255 / 63);
        uint8_t b = (uint8_t)((p & 0x1f) * 255 / 31);
        dst[i * 3 + 0] = r;
        dst[i * 3 + 1] = g;
        dst[i * 3 + 2] = b;
    }
}

int tinyos_png_write_rgb565(const char *path, const uint16_t *fb, int width, int height)
{
    if (!path || !fb || width <= 0 || height <= 0)
        return -1;

    size_t pixels = (size_t)width * (size_t)height;
    uint8_t *rgb = malloc(pixels * 3);
    if (!rgb)
        return -1;

    rgb565_to_rgb888(fb, rgb, (int)pixels);
    int ok = stbi_write_png(path, width, height, 3, rgb, width * 3);
    free(rgb);
    return ok ? 0 : -1;
}
