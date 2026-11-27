#ifndef TINYOS_FBDEV_BLIT_H
#define TINYOS_FBDEV_BLIT_H

#include <stddef.h>
#include <stdint.h>

/* The fbdev backend's mapping and visible geometry. This is deliberately
 * free of LVGL and Linux headers so the exact clear/blit path can be tested
 * on the host without a display device. */
typedef struct {
    uint8_t *memory;
    size_t size;
    size_t stride;
    uint32_t width;
    uint32_t height;
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t bits_per_pixel;
} tinyos_fbdev_geometry;

int tinyos_fbdev_geometry_valid(const tinyos_fbdev_geometry *geometry);
void tinyos_fbdev_clear(const tinyos_fbdev_geometry *geometry);
void tinyos_fbdev_blit_rgb565(const tinyos_fbdev_geometry *geometry,
                              int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                              const uint8_t *pixels);

#endif
