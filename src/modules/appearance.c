#include "module.h"

#include "../core/settings.h"
#include "../ui/fonts.h"
#include "../ui/layout.h"
#include "../ui/scale.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/wallpaper.h"
#include "../ui/widgets.h"

#include <stdio.h>
#include <string.h>

static void open_id(const char *id)
{
    tinyos_shell_show(tinyos_ctx_get(), id);
}

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

static void open_row(lv_event_t *event);

static lv_obj_t *appearance_row(tinyos_ctx *ctx, lv_obj_t *parent, const char *icon,
                                uint32_t accent, const char *title, const char *value,
                                const char *id)
{
    bool large = tinyos_large_text();
    lv_obj_t *row = tinyos_obj(parent, 0, 0, TINYOS_CONTENT_W,
                               large ? TINYOS_ROW_H_LARGE : TINYOS_ROW_ROOMY);
    lv_obj_add_style(row, &tinyos_theme.row, 0);
    lv_obj_set_width(row, LV_PCT(100));
    tinyos_icon(row, icon, accent, TINYOS_ROW_ICON_X, 5, TINYOS_ROW_ICON_SIZE);
    const lv_font_t *row_font = tinyos_text_scale() >= 130
                                    ? &lv_font_montserrat_10
                                    : &lv_font_montserrat_12;
    tinyos_label(row, title, row_font, TINYOS_COLOR_TEXT, TINYOS_ROW_LABEL_X,
                 large ? 2 : 5, large ? 180 : 84, large ? 20 : 15);

    lv_obj_t *pill = tinyos_obj(row,
                                large ? TINYOS_PILL_LARGE_X : TINYOS_PILL_X,
                                large ? TINYOS_PILL_LARGE_Y : 3,
                                large ? TINYOS_PILL_LARGE_W : TINYOS_PILL_W,
                                large ? TINYOS_PILL_LARGE_H : 18);
    lv_obj_add_style(pill, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    lv_obj_t *pill_text = tinyos_label(pill, value, &lv_font_montserrat_10,
                                       TINYOS_COLOR_TEXT, 0, large ? 0 : 3,
                                       large ? TINYOS_PILL_LARGE_W : TINYOS_PILL_W,
                                       large ? TINYOS_PILL_LARGE_H : 12);
    lv_obj_set_style_text_align(pill_text, LV_TEXT_ALIGN_CENTER, 0);

    tinyos_icon(row, LV_SYMBOL_RIGHT, TINYOS_COLOR_DIM, TINYOS_CHEVRON_X,
                large ? TINYOS_CHEVRON_LARGE_Y : 6, TINYOS_CHEVRON_SIZE);
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
    if (!id)
        return;
    if (strcmp(id, "textsize") == 0) {
        tinyos_ctx *ctx = tinyos_ctx_get();
        int next = ctx->settings.text_scale >= 130 ? 100
                   : ctx->settings.text_scale >= 115 ? 130 : 115;
        ctx->settings.text_scale = next;
        tinyos_settings_save(&ctx->settings, TINYOS_SETTINGS_PATH);
        tinyos_shell_reload(ctx, "appearance");
        return;
    }
    open_id(id);
}

static lv_obj_t *appearance_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    lv_obj_t *back = tinyos_back_button(parent, back_cb);
    bool large = tinyos_large_text();
    tinyos_page_title(parent, "Appearance", 0);
    lv_obj_t *list = tinyos_obj(parent, TINYOS_LIST_X, TINYOS_LIST_Y, TINYOS_LIST_W,
                               large ? TINYOS_LIST_H_LARGE : TINYOS_LIST_H);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, TINYOS_ROW_GAP, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_pad_top(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_bottom(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_left(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_right(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollable(list, true);
    lv_obj_set_scrollbar_mode(list, large ? LV_SCROLLBAR_MODE_ON : LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_width(list, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(list, lv_color_hex(TINYOS_COLOR_MUTED), LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(list, LV_OPA_40, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(list, 2, LV_PART_SCROLLBAR);

    appearance_row(ctx, list, LV_SYMBOL_SETTINGS, TINYOS_COLOR_CYAN, "Theme",
                   tinyos_theme_short_name(ctx->settings.theme), "themes");
    char scale_text[8];
    snprintf(scale_text, sizeof(scale_text), "%d%%", ctx->settings.text_scale);
    appearance_row(ctx, list, LV_SYMBOL_KEYBOARD, TINYOS_COLOR_CYAN, "Text size",
                   scale_text, "textsize");
    appearance_row(ctx, list, LV_SYMBOL_IMAGE, TINYOS_COLOR_VIOLET, "Wallpaper",
                   tinyos_wallpaper_name(ctx->settings.wallpaper), "wallpapers");
    appearance_row(ctx, list, LV_SYMBOL_LOOP, TINYOS_COLOR_GREEN, "Motion",
                   ctx->settings.reduce_motion ? "Reduced"
                                               : tinyos_wallpaper_speed_name(ctx->settings.wallpaper_speed),
                   "wallpapers");
    appearance_row(ctx, list, LV_SYMBOL_EDIT, TINYOS_COLOR_AMBER, "Typeface",
                   tinyos_font_family_short_name(ctx->settings.font_family), "typeface");

    tinyos_shell_add_focus(ctx, back);
    return parent;
}

const tinyos_module tinyos_module_appearance = {
    .id = "appearance",
    .name = "Appearance",
    .subtitle = "Make TinyOS yours",
    .icon = LV_SYMBOL_SETTINGS,
    .create = appearance_create,
};
