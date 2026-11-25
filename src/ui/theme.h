#ifndef TINYOS_UI_THEME_H
#define TINYOS_UI_THEME_H

#include <lvgl/lvgl.h>

#define TINYOS_COLOR_BG 0x070B10
#define TINYOS_COLOR_BG_2 0x0B1520
#define TINYOS_COLOR_SURFACE 0x101B25
#define TINYOS_COLOR_SURFACE_2 0x162632
#define TINYOS_COLOR_LINE 0x1D3140
#define TINYOS_COLOR_TEXT 0xF2F8FB
#define TINYOS_COLOR_MUTED 0x8DA3B3
#define TINYOS_COLOR_DIM 0x5B7180
#define TINYOS_COLOR_CYAN 0x3DE1FF
#define TINYOS_COLOR_CYAN_DEEP 0x0C6E86
#define TINYOS_COLOR_VIOLET 0x9A86FF
#define TINYOS_COLOR_GREEN 0x48E6A0
#define TINYOS_COLOR_AMBER 0xFFB454
#define TINYOS_COLOR_RED 0xFF7070

typedef struct {
    lv_style_t card;
    lv_style_t tile;
    lv_style_t focus;
    lv_style_t nav;
    lv_style_t nav_active;
    lv_style_t row;
    lv_style_t pill;
} tinyos_theme_t;

extern tinyos_theme_t tinyos_theme;

void tinyos_theme_init(void);

#endif
