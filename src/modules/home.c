#include "module.h"

#include "../ui/layout.h"
#include "../ui/scale.h"
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

static lv_obj_t *quick_tile(tinyos_ctx *ctx, lv_obj_t *parent, int x, int y,
                            const char *id, const char *symbol, uint32_t accent,
                            const char *label)
{
    bool large = tinyos_large_text();
    int height = large ? TINYOS_QUICK_TILE_H_LARGE : TINYOS_QUICK_TILE_H;
    lv_obj_t *tile = tinyos_obj(parent, x, y, TINYOS_QUICK_TILE_W, height);
    lv_obj_add_style(tile, &tinyos_theme.tile, 0);
    tinyos_gradient(tile, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    tinyos_focus(tile);
    tinyos_add_click(tile, open_cb, (void *)id);
    tinyos_shell_add_focus(ctx, tile);

    /* Keep the 20 px icon + 2 px gap + 14 px label block centred in the
     * tile at both heights, rather than leaving a larger bottom margin. */
    int top = (height - 36) / 2;
    tinyos_icon(tile, symbol, accent, TINYOS_QUICK_ICON_X, top, TINYOS_QUICK_ICON);
    lv_obj_t *text = tinyos_label(tile, label, &lv_font_montserrat_10, TINYOS_COLOR_TEXT,
                                  TINYOS_QUICK_TEXT_X, top + 22, TINYOS_QUICK_TEXT_W,
                                  TINYOS_QUICK_TEXT_H);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    return tile;
}

static lv_obj_t *home_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    bool large = tinyos_large_text();
    int hero_height = large ? 94 : 90;

    lv_obj_t *hero = tinyos_card(parent, TINYOS_CONTENT_X, 0, TINYOS_CONTENT_W,
                                 hero_height);
    tinyos_gradient(hero, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    lv_obj_set_style_border_color(hero, lv_color_hex(TINYOS_COLOR_LINE), 0);
    lv_obj_set_style_border_opa(hero, LV_OPA_COVER, 0);

    lv_obj_t *date = tinyos_label(hero, today(), &lv_font_montserrat_10,
                                  TINYOS_COLOR_CYAN, TINYOS_MARGIN, large ? 8 : 12, 140, 14);
    lv_obj_set_style_text_letter_space(date, 1, 0);

    if (!large) {
        lv_obj_t *orb = tinyos_obj(hero, 168, 2, 28, 28);
        lv_obj_set_style_radius(orb, LV_RADIUS_CIRCLE, 0);
        tinyos_gradient(orb, TINYOS_COLOR_CYAN, TINYOS_COLOR_VIOLET);
        lv_obj_t *orb_inner = tinyos_obj(orb, 3, 3, 22, 22);
        lv_obj_set_style_radius(orb_inner, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(orb_inner, lv_color_hex(TINYOS_COLOR_BG), 0);
        lv_obj_set_style_bg_opa(orb_inner, LV_OPA_COVER, 0);
        lv_obj_t *letter = tinyos_label(orb_inner, "T", &lv_font_montserrat_14,
                                        TINYOS_COLOR_TEXT, 0, 2, 22, 18);
        lv_obj_set_style_text_align(letter, LV_TEXT_ALIGN_CENTER, 0);
    }

    /* The orb sits above the greeting at 100 % so the long “Good afternoon”
     * never runs into it; at 115/130 % the orb is omitted and the greeting
     * takes the taller line. */
    lv_obj_t *greet = tinyos_label(hero, greeting(), &lv_font_montserrat_20,
                                   TINYOS_COLOR_TEXT, TINYOS_MARGIN, large ? 28 : 32,
                                   large ? 196 : 180, large ? 32 : 26);
    /* JetBrains Mono reaches 196 px for “Good afternoon” at 24 px. One px of
     * negative tracking gives every family a real right-hand bearing. */
    if (large)
        lv_obj_set_style_text_letter_space(greet, -1, 0);

    /* At 130 % the full tagline needs a third line and would be cut by the
     * hero. The short form is deliberate secondary copy, not clipped text. */
    lv_obj_t *subtitle = tinyos_label(hero, large ? "Room to grow." : TINYOS_TAGLINE,
                                      &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                      TINYOS_MARGIN, large ? 64 : 60,
                                      large ? 180 : 188, large ? 20 : 26);
    lv_label_set_long_mode(subtitle, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_line_space(subtitle, 0, 0);

    if (!large) {
        lv_obj_t *quick = tinyos_label(parent, "APPS", &lv_font_montserrat_10,
                                       TINYOS_COLOR_MUTED, TINYOS_CONTENT_X, 100,
                                       TINYOS_CONTENT_W, 12);
        lv_obj_set_style_text_letter_space(quick, 1, 0);
    }

    int tile_y = large ? 112 : 120;
    int tile_x = TINYOS_CONTENT_X;
    lv_obj_t *music = quick_tile(ctx, parent, tile_x, tile_y, "music", LV_SYMBOL_AUDIO,
                                 TINYOS_COLOR_CYAN, "Music");
    tile_x += TINYOS_QUICK_TILE_W + TINYOS_QUICK_TILE_GAP;
    lv_obj_t *system = quick_tile(ctx, parent, tile_x, tile_y, "system", LV_SYMBOL_CHARGE,
                                  TINYOS_COLOR_GREEN, "System");
    tile_x += TINYOS_QUICK_TILE_W + TINYOS_QUICK_TILE_GAP;
    lv_obj_t *games = quick_tile(ctx, parent, tile_x, tile_y, "games", LV_SYMBOL_PLAY,
                                 TINYOS_COLOR_VIOLET, "Games");

    tinyos_reveal(hero, 0);
    tinyos_reveal(music, 90);
    tinyos_reveal(system, 125);
    tinyos_reveal(games, 160);
    return parent;
}

const tinyos_module tinyos_module_home = {
    .id = "home",
    .name = "Home",
    .subtitle = "Your starting point",
    .icon = LV_SYMBOL_HOME,
    .create = home_create,
};
