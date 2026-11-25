#include "module.h"

#include "../core/settings.h"
#include "../ui/fonts.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/wallpaper.h"
#include "../ui/widgets.h"

#include <string.h>

static void open_id(const char *id)
{
    tinyos_shell_show(tinyos_ctx_get(), id);
}

static void open_row(lv_event_t *event);

static lv_obj_t *appearance_row(tinyos_ctx *ctx, lv_obj_t *parent, const char *icon,
                                uint32_t accent, const char *title, const char *value,
                                const char *id)
{
    lv_obj_t *row = tinyos_obj(parent, 0, 0, 212, 24);
    lv_obj_add_style(row, &tinyos_theme.row, 0);
    lv_obj_set_width(row, LV_PCT(100));
    tinyos_icon(row, icon, accent, 8, 6, 12);
    tinyos_label(row, title, &lv_font_montserrat_10, TINYOS_COLOR_TEXT, 28, 6, 90, 14);

    lv_obj_t *pill = tinyos_obj(row, 126, 3, 64, 18);
    lv_obj_add_style(pill, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    lv_obj_t *pill_text = tinyos_label(pill, value, &lv_font_montserrat_8, accent,
                                       0, 5, 64, 10);
    lv_obj_set_style_text_align(pill_text, LV_TEXT_ALIGN_CENTER, 0);

    tinyos_icon(row, LV_SYMBOL_RIGHT, TINYOS_COLOR_DIM, 196, 7, 10);
    tinyos_focus(row);
    tinyos_shell_add_focus(ctx, row);
    lv_obj_set_user_data(row, (void *)id);
    lv_obj_add_event_cb(row, open_row, LV_EVENT_CLICKED, NULL);
    return row;
}

static void open_row(lv_event_t *event)
{
    lv_obj_t *row = lv_event_get_target(event);
    const char *id = lv_obj_get_user_data(row);
    if (id)
        open_id(id);
}

static lv_obj_t *appearance_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    tinyos_label(parent, "Appearance", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 14, 0, 212, 24);
    tinyos_label(parent, "Theme, motion and type", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 26, 212, 12);

    lv_obj_t *list = tinyos_obj(parent, 14, 42, 212, 102);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 2, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_scrollable(list, false);

    appearance_row(ctx, list, LV_SYMBOL_SETTINGS, TINYOS_COLOR_CYAN, "Theme",
                   tinyos_theme_name(ctx->settings.theme), "themes");
    appearance_row(ctx, list, LV_SYMBOL_IMAGE, TINYOS_COLOR_VIOLET, "Wallpaper",
                   tinyos_wallpaper_name(ctx->settings.wallpaper), "wallpapers");
    appearance_row(ctx, list, LV_SYMBOL_LOOP, TINYOS_COLOR_GREEN, "Motion",
                   ctx->settings.reduce_motion ? "Reduced"
                                               : tinyos_wallpaper_speed_name(ctx->settings.wallpaper_speed),
                   "wallpapers");
    appearance_row(ctx, list, LV_SYMBOL_EDIT, TINYOS_COLOR_AMBER, "Typeface",
                   tinyos_font_family_name(ctx->settings.font_family), "typeface");

    tinyos_label(parent, "A  OPEN          B  BACK", &lv_font_montserrat_8,
                 TINYOS_COLOR_MUTED, 14, 152, 212, 12);
    return parent;
}

const tinyos_module tinyos_module_appearance = {
    .id = "appearance",
    .name = "Appearance",
    .subtitle = "Make TinyOS yours",
    .icon = LV_SYMBOL_SETTINGS,
    .create = appearance_create,
};
