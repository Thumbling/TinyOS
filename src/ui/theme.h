#ifndef TINYOS_THEME_H
#define TINYOS_THEME_H

#include <stdint.h>
#include <stdbool.h>
#include <lvgl/lvgl.h>

#define TINYOS_THEME_COUNT 25

typedef struct {
    const char *name;
    uint32_t bg;
    uint32_t bg_2;
    uint32_t surface;
    uint32_t surface_2;
    uint32_t line;
    uint32_t text;
    uint32_t muted;
    uint32_t dim;
    uint32_t accent;
    uint32_t accent_2;
    uint32_t success;
    uint32_t warning;
    uint32_t danger;
    uint32_t on_accent;
    uint8_t wallpaper;
} tinyos_theme_definition_t;

typedef struct {
    lv_style_t card;
    lv_style_t tile;
    lv_style_t focus;
    lv_style_t pressed;
    lv_style_t nav;
    lv_style_t nav_item;
    lv_style_t nav_active;
    lv_style_t row;
    lv_style_t pill;

    uint32_t bg;
    uint32_t bg_2;
    uint32_t surface;
    uint32_t surface_2;
    uint32_t line;
    uint32_t text;
    uint32_t muted;
    uint32_t dim;
    uint32_t accent;
    uint32_t accent_2;
    uint32_t accent_deep;
    uint32_t success;
    uint32_t warning;
    uint32_t danger;
    uint32_t on_accent;
    int index;
} tinyos_theme_t;

extern tinyos_theme_t tinyos_theme;

#define TINYOS_COLOR_BG (tinyos_theme.bg)
#define TINYOS_COLOR_BG_2 (tinyos_theme.bg_2)
#define TINYOS_COLOR_SURFACE (tinyos_theme.surface)
#define TINYOS_COLOR_SURFACE_2 (tinyos_theme.surface_2)
#define TINYOS_COLOR_LINE (tinyos_theme.line)
#define TINYOS_COLOR_TEXT (tinyos_theme.text)
#define TINYOS_COLOR_MUTED (tinyos_theme.muted)
#define TINYOS_COLOR_DIM (tinyos_theme.dim)
#define TINYOS_COLOR_CYAN (tinyos_theme.accent)
#define TINYOS_COLOR_CYAN_DEEP (tinyos_theme.accent_deep)
#define TINYOS_COLOR_VIOLET (tinyos_theme.accent_2)
#define TINYOS_COLOR_GREEN (tinyos_theme.success)
#define TINYOS_COLOR_AMBER (tinyos_theme.warning)
#define TINYOS_COLOR_RED (tinyos_theme.danger)
#define TINYOS_COLOR_ON_ACCENT (tinyos_theme.on_accent)

/* Applies a palette and rebuilds every shared style. Safe to call at runtime. */
void tinyos_theme_apply(int index);
void tinyos_theme_init(void);
int tinyos_theme_index(void);
const char *tinyos_theme_name(int index);
const char *tinyos_theme_short_name(int index);
const tinyos_theme_definition_t *tinyos_theme_definition(int index);

/* Linear sRGB interpolation; ratio_b is 0 (a) .. 255 (b). */
uint32_t tinyos_color_mix(uint32_t a, uint32_t b, uint8_t ratio_b);

#endif
