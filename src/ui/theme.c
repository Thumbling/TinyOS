#include "theme.h"

#include <stdbool.h>

tinyos_theme_t tinyos_theme;
static bool theme_ready;

void tinyos_theme_init(void)
{
    if (theme_ready)
        return;
    theme_ready = true;

    lv_style_init(&tinyos_theme.card);
    lv_style_set_bg_color(&tinyos_theme.card, lv_color_hex(TINYOS_COLOR_SURFACE));
    lv_style_set_bg_opa(&tinyos_theme.card, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.card, 20);
    lv_style_set_border_width(&tinyos_theme.card, 1);
    lv_style_set_border_color(&tinyos_theme.card, lv_color_hex(TINYOS_COLOR_LINE));
    lv_style_set_border_opa(&tinyos_theme.card, LV_OPA_70);
    lv_style_set_shadow_width(&tinyos_theme.card, 18);
    lv_style_set_shadow_color(&tinyos_theme.card, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&tinyos_theme.card, LV_OPA_20);
    lv_style_set_shadow_offset_y(&tinyos_theme.card, 7);
    lv_style_set_pad_all(&tinyos_theme.card, 0);

    lv_style_init(&tinyos_theme.tile);
    lv_style_set_bg_color(&tinyos_theme.tile, lv_color_hex(TINYOS_COLOR_SURFACE));
    lv_style_set_bg_opa(&tinyos_theme.tile, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.tile, 16);
    lv_style_set_border_width(&tinyos_theme.tile, 1);
    lv_style_set_border_color(&tinyos_theme.tile, lv_color_hex(TINYOS_COLOR_LINE));
    lv_style_set_border_opa(&tinyos_theme.tile, LV_OPA_60);
    lv_style_set_shadow_width(&tinyos_theme.tile, 12);
    lv_style_set_shadow_color(&tinyos_theme.tile, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&tinyos_theme.tile, LV_OPA_20);
    lv_style_set_shadow_offset_y(&tinyos_theme.tile, 4);
    lv_style_set_pad_all(&tinyos_theme.tile, 0);

    lv_style_init(&tinyos_theme.focus);
    lv_style_set_border_width(&tinyos_theme.focus, 2);
    lv_style_set_border_color(&tinyos_theme.focus, lv_color_hex(TINYOS_COLOR_CYAN));
    lv_style_set_border_opa(&tinyos_theme.focus, LV_OPA_COVER);

    lv_style_init(&tinyos_theme.nav);
    lv_style_set_bg_color(&tinyos_theme.nav, lv_color_hex(0x0B141C));
    lv_style_set_bg_opa(&tinyos_theme.nav, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.nav, 14);
    lv_style_set_border_width(&tinyos_theme.nav, 1);
    lv_style_set_border_color(&tinyos_theme.nav, lv_color_hex(0x172732));
    lv_style_set_border_opa(&tinyos_theme.nav, LV_OPA_80);
    lv_style_set_shadow_width(&tinyos_theme.nav, 14);
    lv_style_set_shadow_color(&tinyos_theme.nav, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&tinyos_theme.nav, LV_OPA_30);
    lv_style_set_shadow_offset_y(&tinyos_theme.nav, 4);

    lv_style_init(&tinyos_theme.nav_active);
    lv_style_set_bg_color(&tinyos_theme.nav_active, lv_color_hex(0x122A36));
    lv_style_set_bg_opa(&tinyos_theme.nav_active, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.nav_active, 11);

    lv_style_init(&tinyos_theme.row);
    lv_style_set_bg_color(&tinyos_theme.row, lv_color_hex(0x111D27));
    lv_style_set_bg_opa(&tinyos_theme.row, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.row, 12);
    lv_style_set_border_width(&tinyos_theme.row, 0);
    lv_style_set_pad_all(&tinyos_theme.row, 0);

    lv_style_init(&tinyos_theme.pill);
    lv_style_set_bg_color(&tinyos_theme.pill, lv_color_hex(0x1A2B37));
    lv_style_set_bg_opa(&tinyos_theme.pill, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.pill, LV_RADIUS_CIRCLE);
    lv_style_set_border_width(&tinyos_theme.pill, 0);
    lv_style_set_pad_all(&tinyos_theme.pill, 0);
}
