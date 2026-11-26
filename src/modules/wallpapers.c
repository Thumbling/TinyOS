#include "module.h"

#include "../core/settings.h"
#include "../platform.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/wallpaper.h"
#include "../ui/widgets.h"

#include <stdint.h>

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

static tinyos_wallpaper *preview_wallpaper;
static lv_obj_t *preview_card;
static lv_obj_t *style_value;
static lv_obj_t *speed_value;

static int effective_speed(const tinyos_ctx *ctx)
{
    return ctx->settings.reduce_motion ? 0 : ctx->settings.wallpaper_speed;
}

static void set_pill_text(lv_obj_t *pill, const char *text)
{
    if (!pill)
        return;
    lv_obj_t *label = lv_obj_get_child(pill, 0);
    if (label)
        lv_label_set_text(label, text);
}

static void update_values(void)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    set_pill_text(style_value, tinyos_wallpaper_name(ctx->settings.wallpaper));
    set_pill_text(speed_value, tinyos_wallpaper_speed_name(ctx->settings.wallpaper_speed));
}

static void rebuild_preview(void)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    if (!preview_card)
        return;
    if (preview_wallpaper) {
        tinyos_wallpaper_destroy(preview_wallpaper);
        preview_wallpaper = NULL;
    }
    preview_wallpaper = tinyos_wallpaper_create(preview_card, 212, 54,
                                                ctx->settings.wallpaper,
                                                effective_speed(ctx), true);
}

static void refresh_wallpaper(void)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    update_values();
    rebuild_preview();
    tinyos_shell_set_wallpaper(ctx, ctx->settings.wallpaper, effective_speed(ctx));
}

static void change_style(int delta)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    int next = (ctx->settings.wallpaper + delta + TINYOS_WALLPAPER_COUNT)
               % TINYOS_WALLPAPER_COUNT;
    ctx->settings.wallpaper = next;
    tinyos_settings_save(&ctx->settings, TINYOS_SETTINGS_PATH);
    refresh_wallpaper();
}

static void change_speed(int delta)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    int next = (ctx->settings.wallpaper_speed + delta + TINYOS_WALLPAPER_SPEEDS)
               % TINYOS_WALLPAPER_SPEEDS;
    ctx->settings.wallpaper_speed = next;
    tinyos_settings_save(&ctx->settings, TINYOS_SETTINGS_PATH);
    refresh_wallpaper();
}

static void style_clicked(lv_event_t *event)
{
    (void)event;
    change_style(1);
}

static void speed_clicked(lv_event_t *event)
{
    (void)event;
    change_speed(1);
}

static void style_key(lv_event_t *event)
{
    uint32_t key = lv_event_get_key(event);
    if (key == TINYOS_KEY_PREV)
        change_style(-1);
    else if (key == TINYOS_KEY_NEXT)
        change_style(1);
}

static void speed_key(lv_event_t *event)
{
    uint32_t key = lv_event_get_key(event);
    if (key == TINYOS_KEY_PREV)
        change_speed(-1);
    else if (key == TINYOS_KEY_NEXT)
        change_speed(1);
}

static lv_obj_t *option_row(tinyos_ctx *ctx, lv_obj_t *parent, int y,
                            const char *icon, uint32_t color, const char *text,
                            const char *value, lv_event_cb_t click,
                            lv_event_cb_t key)
{
    lv_obj_t *row = tinyos_obj(parent, 14, y, 212, 22);
    lv_obj_add_style(row, &tinyos_theme.row, 0);
    tinyos_icon(row, icon, color, 8, 4, 14);
    tinyos_label(row, text, &lv_font_montserrat_12, TINYOS_COLOR_TEXT, 28, 2, 74, 15);
    lv_obj_t *pill = tinyos_obj(row, 106, 2, 72, 18);
    lv_obj_add_style(pill, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    lv_obj_t *label = tinyos_label(pill, value, &lv_font_montserrat_10, color,
                                   0, 3, 72, 12);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    tinyos_icon(row, LV_SYMBOL_RIGHT, TINYOS_COLOR_DIM, 186, 5, 12);
    tinyos_focus(row);
    tinyos_shell_add_focus(ctx, row);
    lv_obj_add_event_cb(row, click, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(row, key, LV_EVENT_KEY, NULL);
    return pill;
}

static lv_obj_t *wallpapers_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    preview_wallpaper = NULL;

    lv_obj_t *back = tinyos_obj(parent, 10, 2, 26, 26);
    lv_obj_add_style(back, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(back, 10, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    tinyos_focus(back);
    tinyos_add_click(back, back_cb, NULL);
    tinyos_icon(back, LV_SYMBOL_LEFT, TINYOS_COLOR_MUTED, 0, 0, 26);
    lv_obj_center(lv_obj_get_child(back, 0));
    tinyos_label(parent, "Wallpapers", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 44, 2, 170, 24);
    tinyos_label(parent, "Animated backdrop", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 44, 25, 180, 12);

    preview_card = tinyos_card(parent, 14, 42, 212, 54);
    lv_obj_set_style_border_width(preview_card, 1, 0);
    lv_obj_set_style_clip_corner(preview_card, true, 0);

    style_value = option_row(ctx, parent, 100, LV_SYMBOL_IMAGE, TINYOS_COLOR_CYAN,
                             "Wallpaper", tinyos_wallpaper_name(ctx->settings.wallpaper),
                             style_clicked, style_key);
    speed_value = option_row(ctx, parent, 124, LV_SYMBOL_LOOP, TINYOS_COLOR_GREEN,
                             "Speed", tinyos_wallpaper_speed_name(ctx->settings.wallpaper_speed),
                             speed_clicked, speed_key);
    tinyos_label(parent, "X / Y adjust     A cycle", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 152, 212, 13);

    rebuild_preview();
    tinyos_shell_add_focus(ctx, back);
    return parent;
}

const tinyos_module tinyos_module_wallpapers = {
    .id = "wallpapers",
    .name = "Wallpapers",
    .subtitle = "Motion with restraint",
    .icon = LV_SYMBOL_IMAGE,
    .create = wallpapers_create,
};
