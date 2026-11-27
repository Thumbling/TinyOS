#include "fbdev_blit.h"

#include <string.h>

int tinyos_fbdev_geometry_valid(const tinyos_fbdev_geometry *geometry)
{
    if (!geometry || !geometry->memory || geometry->size == 0 ||
        geometry->stride == 0 || geometry->width == 0 || geometry->height == 0)
        return 0;

    if (geometry->bits_per_pixel != 16 && geometry->bits_per_pixel != 32)
        return 0;

    size_t row_bytes = ((size_t)geometry->xoffset + geometry->width) *
                       (geometry->bits_per_pixel / 8);
    size_t visible_bytes = ((size_t)geometry->yoffset + geometry->height) *
                           geometry->stride;

    return geometry->stride >= row_bytes && geometry->size >= visible_bytes;
}

void tinyos_fbdev_clear(const tinyos_fbdev_geometry *geometry)
{
    if (geometry && geometry->memory && geometry->size > 0)
        memset(geometry->memory, 0, geometry->size);
}

void tinyos_fbdev_blit_rgb565(const tinyos_fbdev_geometry *geometry,
                              int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                              const uint8_t *pixels)
{
    if (!geometry || !geometry->memory || !pixels)
        return;

    int32_t width = x2 - x1 + 1;
    int32_t height = y2 - y1 + 1;
    if (width <= 0 || height <= 0)
        return;

    /* LVGL renders inside the display. Keep the old fail-safe: an area that
     * crosses the panel boundary is not partially painted. */
    if (x1 < 0 || y1 < 0 || x2 >= (int32_t)geometry->width ||
        y2 >= (int32_t)geometry->height)
        return;

    for (int32_t row = 0; row < height; row++) {
        int32_t y = y1 + row;
        size_t destination_offset =
            ((size_t)y + geometry->yoffset) * geometry->stride +
            ((size_t)x1 + geometry->xoffset) * (geometry->bits_per_pixel / 8);
        const uint8_t *source = pixels + (size_t)row * (size_t)width * 2;

        if (geometry->bits_per_pixel == 16) {
            memcpy(geometry->memory + destination_offset, source, (size_t)width * 2);
        } else if (geometry->bits_per_pixel == 32) {
            uint32_t *destination = (uint32_t *)(geometry->memory + destination_offset);
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
}
