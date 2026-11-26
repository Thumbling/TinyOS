#include "module.h"

#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"
#include "../version.h"

#include <ctype.h>
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
        return "Good morning";
    if (hour < 18)
        return "Good afternoon";
    return "Good evening";
}

/* "MON 24 NOV": the hero says something the shell does not, instead of the
 * product name a second time. */
static const char *today(void)
{
    static char buf[24];
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (!local || strftime(buf, sizeof(buf), "%a %d %b", local) == 0)
        return "TINYOS";
    for (char *p = buf; *p; p++)
        *p = (char)toupper((unsigned char)*p);
    return buf;
}

static lv_obj_t *quick_tile(tinyos_ctx *ctx, lv_obj_t *parent, int x, const char *id,
                            const char *symbol, uint32_t accent, const char *label)
{
    lv_obj_t *tile = tinyos_obj(parent, x, 120, 64, 44);
    lv_obj_add_style(tile, &tinyos_theme.tile, 0);
    tinyos_gradient(tile, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    tinyos_focus(tile);
    tinyos_add_click(tile, open_cb, (void *)id);
    tinyos_shell_add_focus(ctx, tile);

    tinyos_icon(tile, symbol, accent, 22, 6, 20);
    lv_obj_t *text = tinyos_label(tile, label, &lv_font_montserrat_10, TINYOS_COLOR_TEXT,
                                  0, 26, 64, 14);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    return tile;
}

static lv_obj_t *home_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    lv_obj_t *hero = tinyos_card(parent, 14, 0, 212, 88);
    tinyos_gradient(hero, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    lv_obj_set_style_border_color(hero, lv_color_hex(TINYOS_COLOR_LINE), 0);
    lv_obj_set_style_border_opa(hero, LV_OPA_COVER, 0);

    lv_obj_t *date = tinyos_label(hero, today(), &lv_font_montserrat_10,
                                  TINYOS_COLOR_CYAN, 16, 12, 140, 14);
    lv_obj_set_style_text_letter_space(date, 1, 0);

    lv_obj_t *orb = tinyos_obj(hero, 168, 10, 28, 28);
    lv_obj_set_style_radius(orb, LV_RADIUS_CIRCLE, 0);
    tinyos_gradient(orb, TINYOS_COLOR_CYAN, TINYOS_COLOR_VIOLET);
    lv_obj_t *orb_inner = tinyos_obj(orb, 3, 3, 22, 22);
    lv_obj_set_style_radius(orb_inner, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(orb_inner, lv_color_hex(TINYOS_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(orb_inner, LV_OPA_COVER, 0);
    lv_obj_t *letter = tinyos_label(orb_inner, "T", &lv_font_montserrat_14,
                                    TINYOS_COLOR_TEXT, 0, 2, 22, 18);
    lv_obj_set_style_text_align(letter, LV_TEXT_ALIGN_CENTER, 0);

    tinyos_label(hero, greeting(), &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 16, 30, 180, 26);
    lv_obj_t *subtitle = tinyos_label(hero, TINYOS_TAGLINE, &lv_font_montserrat_10,
                                      TINYOS_COLOR_MUTED, 16, 60, 188, 26);
    lv_label_set_long_mode(subtitle, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_line_space(subtitle, 0, 0);

    lv_obj_t *quick = tinyos_label(parent, "QUICK ACCESS", &lv_font_montserrat_10,
                                   TINYOS_COLOR_MUTED, 15, 98, 200, 12);
    lv_obj_set_style_text_letter_space(quick, 1, 0);

    /* Only what the tab bar cannot reach: Music, System and About. */
    quick_tile(ctx, parent, 14, "music", LV_SYMBOL_AUDIO, TINYOS_COLOR_CYAN, "Music");
    quick_tile(ctx, parent, 88, "system", LV_SYMBOL_CHARGE, TINYOS_COLOR_GREEN, "System");
    quick_tile(ctx, parent, 162, "about", LV_SYMBOL_FILE, TINYOS_COLOR_VIOLET, "About");

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
