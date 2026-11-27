#include "module.h"

#include "../core/settings.h"
#include "../ui/fonts.h"
#include "../ui/layout.h"
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
    lv_obj_t *back = tinyos_back_button(parent, back_cb);
    tinyos_page_title(parent, "Typeface", 0);

    lv_obj_t *list = tinyos_obj(parent, TINYOS_LIST_X, TINYOS_LIST_Y, TINYOS_LIST_W,
                                TINYOS_LIST_H);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_pad_top(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_bottom(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_left(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_right(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    /* All six families fit at every scale, so no scrollbar is needed. */
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_width(list, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(list, lv_color_hex(TINYOS_COLOR_MUTED), LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(list, LV_OPA_40, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(list, 2, LV_PART_SCROLLBAR);
    lv_obj_set_scrollable(list, true);

    for (int i = 0; i < TINYOS_FONT_FAMILY_COUNT; i++) {
        bool selected = i == ctx->settings.font_family;
        lv_obj_t *row = tinyos_obj(list, 0, 0, TINYOS_CONTENT_W, TINYOS_ROW_H);
        lv_obj_set_width(row, LV_PCT(100));
        lv_obj_add_style(row, &tinyos_theme.row, 0);
        lv_obj_set_style_radius(row, TINYOS_ROW_RADIUS, 0);
        if (selected)
            lv_obj_set_style_bg_color(row, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);

        if (selected) {
            lv_obj_t *mark = tinyos_obj(row, 4, large ? 5 : 4, 3, 13);
            lv_obj_set_style_radius(mark, 2, 0);
            lv_obj_set_style_bg_color(mark, lv_color_hex(TINYOS_COLOR_CYAN), 0);
            lv_obj_set_style_bg_opa(mark, LV_OPA_COVER, 0);
        }

        const lv_font_t *name_font = tinyos_text_scale() >= 130
                                         ? &lv_font_montserrat_10
                                         : &lv_font_montserrat_12;
        lv_obj_t *name = tinyos_label(row, tinyos_font_family_name(i),
                                      name_font,
                                      selected ? TINYOS_COLOR_TEXT : TINYOS_COLOR_MUTED,
                                      16, large ? 0 : 2, large ? 118 : 100,
                                      large ? 20 : 15);
        lv_obj_set_style_text_color(name, lv_color_hex(selected ? TINYOS_COLOR_TEXT
                                                                : TINYOS_COLOR_MUTED),
                                    0);

        int sample_px = 12;
        lv_obj_t *sample = tinyos_label(row, "Aa 012",
                                        tinyos_font_family(i, sample_px),
                                        selected ? TINYOS_COLOR_CYAN : TINYOS_COLOR_MUTED,
                                        large ? 136 : 120, large ? 0 : 2,
                                        large ? 52 : 58, large ? 20 : 16);
        /* A specimen must stay in its candidate family at 12 px. tinyos_label
         * normally follows the active UI typeface/size, which made the first
         * (Montserrat) specimen switch to the selected family and ellipsise. */
        lv_obj_set_style_text_font(sample, tinyos_font_family(i, sample_px), 0);

        if (selected)
            /* Keep the status mark on the same 5 px right padding at every
             * text scale; the sample column grows into the extra room. */
            tinyos_icon(row, LV_SYMBOL_OK, TINYOS_COLOR_CYAN, TINYOS_CHEVRON_X + 6,
                        large ? 5 : 4, TINYOS_CHEVRON_SIZE);

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
