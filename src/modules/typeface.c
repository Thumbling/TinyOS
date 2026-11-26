#include "module.h"

#include "../core/settings.h"
#include "../ui/fonts.h"
#include "../ui/scale.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdint.h>

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

static void font_clicked(lv_event_t *event)
{
    lv_obj_t *row = lv_event_get_target(event);
    int family = (int)(intptr_t)lv_obj_get_user_data(row);
    tinyos_ctx *ctx = tinyos_ctx_get();
    ctx->settings.font_family = family;
    tinyos_fonts_select(family);
    tinyos_settings_save(&ctx->settings, TINYOS_SETTINGS_PATH);
    tinyos_shell_reload(ctx, "appearance");
}

static lv_obj_t *typeface_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    bool large = tinyos_large_text();
    lv_obj_t *back = tinyos_obj(parent, 10, 2, 26, 26);
    lv_obj_add_style(back, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(back, 10, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    tinyos_focus(back);
    tinyos_add_click(back, back_cb, NULL);
    tinyos_icon(back, LV_SYMBOL_LEFT, TINYOS_COLOR_MUTED, 0, 0, 26);
    lv_obj_center(lv_obj_get_child(back, 0));
    tinyos_label(parent, "Typeface", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 44, 2, 170, 24);
    if (!large)
        tinyos_label(parent, "Aa 0123  -  pick a voice", &lv_font_montserrat_10,
                     TINYOS_COLOR_MUTED, 44, 25, 180, 12);

    lv_obj_t *list = tinyos_obj(parent, 14, large ? 38 : 42, 212, large ? 136 : 132);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, large ? 2 : 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollable(list, true);

    for (int i = 0; i < TINYOS_FONT_FAMILY_COUNT; i++) {
        bool selected = i == ctx->settings.font_family;
        lv_obj_t *row = tinyos_obj(list, 0, 0, 212, large ? 23 : 21);
        lv_obj_set_width(row, LV_PCT(100));
        lv_obj_add_style(row, &tinyos_theme.row, 0);
        lv_obj_set_style_radius(row, 9, 0);
        if (selected)
            lv_obj_set_style_bg_color(row, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);

        if (selected) {
            lv_obj_t *mark = tinyos_obj(row, 4, large ? 5 : 4, 3, 13);
            lv_obj_set_style_radius(mark, 2, 0);
            lv_obj_set_style_bg_color(mark, lv_color_hex(TINYOS_COLOR_CYAN), 0);
            lv_obj_set_style_bg_opa(mark, LV_OPA_COVER, 0);
        }

        lv_obj_t *name = tinyos_label(row, tinyos_font_family_name(i),
                                      &lv_font_montserrat_12,
                                      selected ? TINYOS_COLOR_TEXT : TINYOS_COLOR_MUTED,
                                      16, 2, large ? 118 : 100, large ? 20 : 15);
        lv_obj_set_style_text_color(name, lv_color_hex(selected ? TINYOS_COLOR_TEXT
                                                                : TINYOS_COLOR_MUTED),
                                    0);

        int sample_px = tinyos_large_text() ? tinyos_text_px(10) : 12;
        lv_obj_t *sample = tinyos_label(row, "Aa 012",
                                        tinyos_font_family(i, sample_px),
                                        selected ? TINYOS_COLOR_CYAN : TINYOS_COLOR_MUTED,
                                        large ? 138 : 120, 2, large ? 60 : 58,
                                        large ? 20 : 16);
        (void)sample;

        if (selected)
            tinyos_icon(row, LV_SYMBOL_OK, TINYOS_COLOR_CYAN, large ? 198 : 186,
                        large ? 5 : 4, 12);

        tinyos_focus(row);
        tinyos_shell_add_focus(ctx, row);
        lv_obj_set_user_data(row, (void *)(intptr_t)i);
        lv_obj_add_event_cb(row, font_clicked, LV_EVENT_CLICKED, NULL);
    }
    tinyos_shell_add_focus(ctx, back);
    return parent;
}

const tinyos_module tinyos_module_typeface = {
    .id = "typeface",
    .name = "Typeface",
    .subtitle = "Six families, all sizes",
    .icon = LV_SYMBOL_EDIT,
    .create = typeface_create,
};
