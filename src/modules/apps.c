#include "module.h"

#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

static void open_cb(lv_event_t *event)
{
    const char *id = lv_event_get_user_data(event);
    tinyos_shell_show(tinyos_ctx_get(), id);
}

static void app_tile(tinyos_ctx *ctx, lv_obj_t *parent, int x, int y, const char *id,
                     const char *symbol, uint32_t accent, const char *title,
                     const char *subtitle)
{
    lv_obj_t *tile = tinyos_obj(parent, x, y, 102, 56);
    lv_obj_add_style(tile, &tinyos_theme.tile, 0);
    tinyos_gradient(tile, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    tinyos_focus(tile);
    tinyos_add_click(tile, open_cb, (void *)id);
    tinyos_shell_add_focus(ctx, tile);

    lv_obj_t *badge = tinyos_obj(tile, 8, 14, 28, 28);
    lv_obj_set_style_radius(badge, 9, 0);
    lv_obj_set_style_bg_color(badge, lv_color_hex(accent), 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_20, 0);
    tinyos_icon(badge, symbol, accent, 4, 4, 20);

    tinyos_label(tile, title, &lv_font_montserrat_10, TINYOS_COLOR_TEXT, 42, 11, 52, 12);
    tinyos_label(tile, subtitle, &lv_font_montserrat_8, TINYOS_COLOR_MUTED, 42, 26, 52, 10);

    lv_obj_t *chevron = tinyos_icon(tile, LV_SYMBOL_RIGHT, TINYOS_COLOR_DIM, 86, 22, 10);
    lv_obj_set_style_text_align(chevron, LV_TEXT_ALIGN_CENTER, 0);
}

static lv_obj_t *apps_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    tinyos_label(parent, "Library", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 14, 2, 212, 24);
    tinyos_label(parent, "Everything in one place.", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 15, 25, 212, 12);

    app_tile(ctx, parent, 14, 44, "music", LV_SYMBOL_AUDIO, TINYOS_COLOR_CYAN,
             "Music", "WAV player");
    app_tile(ctx, parent, 124, 44, "system", LV_SYMBOL_DRIVE, TINYOS_COLOR_GREEN,
             "System", "Live stats");
    app_tile(ctx, parent, 14, 104, "settings", LV_SYMBOL_SETTINGS, TINYOS_COLOR_VIOLET,
             "Settings", "Tune");
    app_tile(ctx, parent, 124, 104, "about", LV_SYMBOL_FILE, TINYOS_COLOR_AMBER,
             "About", "Version");

    tinyos_label(parent, "Modular by design.", &lv_font_montserrat_8, TINYOS_COLOR_DIM,
                 14, 164, 212, 10);

    for (int i = 0; i < 5; i++) {
        lv_obj_t *child = lv_obj_get_child(parent, i + 2);
        if (child)
            tinyos_reveal(child, i * 40);
    }
    return parent;
}

const tinyos_module tinyos_module_apps = {
    .id = "apps",
    .name = "Library",
    .subtitle = "All installed modules",
    .icon = LV_SYMBOL_LIST,
    .create = apps_create,
};
