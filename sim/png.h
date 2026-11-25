#ifndef TINYOS_SIM_PNG_H
#define TINYOS_SIM_PNG_H

#include <stdint.h>

/* Write an RGB565 framebuffer as an 8-bit RGB PNG. Returns 0 on success. */
int tinyos_png_write_rgb565(const char *path, const uint16_t *fb, int width, int height);

#endif
