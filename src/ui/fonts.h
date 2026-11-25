#ifndef TINYOS_FONTS_H
#define TINYOS_FONTS_H

#include <lvgl/lvgl.h>

#define TINYOS_FONT_FAMILY_COUNT 6
#define TINYOS_FONT_SIZE_COUNT 8

void tinyos_fonts_select(int family);
int tinyos_fonts_active(void);
const char *tinyos_font_family_name(int family);
const lv_font_t *tinyos_font_ui(int size);
const lv_font_t *tinyos_font_family(int family, int size);
const lv_font_t *tinyos_font_map(const lv_font_t *requested);

#endif
