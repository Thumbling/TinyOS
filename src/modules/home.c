#include "module.h"

#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"
#include "../version.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static void open_cb(lv_event_t *event)
{
    const char *id = lv_event_get_user_data(event);
    tinyos_shell_show(tinyos_ctx_get(), id);
}

static const char *greeting(void)
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    int hour = local ? local->tm_hour : 20;
    if (hour < 12)
        return "GOOD MORNING";
    if (hour < 18)
        return "GOOD AFTERNOON";
    return "GOOD EVENING";
}

static lv_obj_t *quick_tile(tinyos_ctx *ctx, lv_obj_t *parent, int x, const char *id,
                            const char *symbol, uint32_t accent, const char *label)
{
    lv_obj_t *tile = tinyos_obj(parent, x, 120, 64, 50);
    lv_obj_add_style(tile, &tinyos_theme.tile, 0);
    tinyos_gradient(tile, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    tinyos_focus(tile);
    tinyos_add_click(tile, open_cb, (void *)id);
    tinyos_shell_add_focus(ctx, tile);

    tinyos_icon(tile, symbol, accent, 22, 5, 20);
    lv_obj_t *text = tinyos_label(tile, label, &lv_font_montserrat_8, TINYOS_COLOR_TEXT,
                                  0, 29, 64, 10);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    return tile;
}

static lv_obj_t *home_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    lv_obj_t *hero = tinyos_card(parent, 14, 0, 212, 94);
    tinyos_gradient(hero, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    lv_obj_set_style_border_color(hero, lv_color_hex(TINYOS_COLOR_LINE), 0);
    lv_obj_set_style_border_opa(hero, LV_OPA_COVER, 0);

    lv_obj_t *eyebrow = tinyos_label(hero, greeting(), &lv_font_montserrat_8,
                                     TINYOS_COLOR_CYAN, 16, 11, 128, 10);
    lv_obj_set_style_text_letter_space(eyebrow, 1, 0);

    tinyos_label(hero, "TinyOS", &lv_font_montserrat_28, TINYOS_COLOR_TEXT, 14, 20, 130, 32);

    lv_obj_t *subtitle = tinyos_label(hero, TINYOS_TAGLINE, &lv_font_montserrat_10,
                                      TINYOS_COLOR_MUTED, 16, 54, 128, 22);
    lv_label_set_long_mode(subtitle, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_line_space(subtitle, -1, 0);

    lv_obj_t *cta = tinyos_obj(hero, 16, 76, 88, 17);
    lv_obj_add_style(cta, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(cta, lv_color_hex(TINYOS_COLOR_CYAN), 0);
    lv_obj_set_style_shadow_width(cta, 12, 0);
    lv_obj_set_style_shadow_color(cta, lv_color_hex(TINYOS_COLOR_CYAN), 0);
    lv_obj_set_style_shadow_opa(cta, LV_OPA_30, 0);
    tinyos_focus(cta);
    tinyos_add_click(cta, open_cb, (void *)"apps");
    tinyos_shell_add_focus(ctx, cta);
    lv_obj_t *cta_text = tinyos_label(cta, "OPEN LIBRARY", &lv_font_montserrat_8,
                                      TINYOS_COLOR_ON_ACCENT, 0, 4, 88, 10);
    lv_obj_set_style_text_align(cta_text, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *orb = tinyos_obj(hero, 152, 14, 54, 54);
    lv_obj_set_style_radius(orb, LV_RADIUS_CIRCLE, 0);
    tinyos_gradient(orb, TINYOS_COLOR_CYAN, TINYOS_COLOR_VIOLET);
    lv_obj_set_style_shadow_width(orb, 18, 0);
    lv_obj_set_style_shadow_color(orb, lv_color_hex(TINYOS_COLOR_CYAN), 0);
    lv_obj_set_style_shadow_opa(orb, LV_OPA_30, 0);
    lv_obj_t *orb_inner = tinyos_obj(orb, 5, 5, 44, 44);
    lv_obj_set_style_radius(orb_inner, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(orb_inner, lv_color_hex(TINYOS_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(orb_inner, LV_OPA_COVER, 0);
    tinyos_label(orb_inner, "T", &lv_font_montserrat_24, TINYOS_COLOR_TEXT, 0, 5, 44, 30);
    lv_obj_t *orb_letter = lv_obj_get_child(orb_inner, 0);
    lv_obj_set_style_text_align(orb_letter, LV_TEXT_ALIGN_CENTER, 0);

    tinyos_label(hero, "v" TINYOS_VERSION, &lv_font_montserrat_8, TINYOS_COLOR_DIM,
                 158, 73, 44, 10);

    lv_obj_t *quick = tinyos_label(parent, "QUICK ACCESS", &lv_font_montserrat_8,
                                   TINYOS_COLOR_MUTED, 15, 104, 200, 10);
    lv_obj_set_style_text_letter_space(quick, 1, 0);

    quick_tile(ctx, parent, 14, "music", LV_SYMBOL_AUDIO, TINYOS_COLOR_CYAN, "Music");
    quick_tile(ctx, parent, 88, "apps", LV_SYMBOL_LIST, TINYOS_COLOR_VIOLET, "Library");
    quick_tile(ctx, parent, 162, "settings", LV_SYMBOL_SETTINGS, TINYOS_COLOR_AMBER,
               "Settings");

    tinyos_reveal(hero, 0);
    tinyos_reveal(quick, 55);
    for (int i = 0; i < 3; i++) {
        lv_obj_t *tile = lv_obj_get_child(parent, 2 + i);
        if (tile)
            tinyos_reveal(tile, 90 + i * 35);
    }
    return parent;
}

const tinyos_module tinyos_module_home = {
    .id = "home",
    .name = "Home",
    .subtitle = "Your starting point",
    .icon = LV_SYMBOL_HOME,
    .create = home_create,
};
