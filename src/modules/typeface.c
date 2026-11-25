#include "module.h"

#include "../core/settings.h"
#include "../ui/fonts.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdint.h>

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
    tinyos_label(parent, "Typeface", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 14, 0, 212, 24);
    tinyos_label(parent, "Aa 0123  -  pick a voice", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 26, 212, 12);

    for (int i = 0; i < TINYOS_FONT_FAMILY_COUNT; i++) {
        bool selected = i == ctx->settings.font_family;
        lv_obj_t *row = tinyos_obj(parent, 14, 42 + i * 22, 212, 20);
        lv_obj_add_style(row, &tinyos_theme.row, 0);
        if (selected)
            lv_obj_set_style_bg_color(row, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);

        lv_obj_t *mark = tinyos_obj(row, 4, 4, 3, 12);
        lv_obj_set_style_radius(mark, 2, 0);
        lv_obj_set_style_bg_color(mark, lv_color_hex(selected ? TINYOS_COLOR_CYAN
                                                               : TINYOS_COLOR_DIM),
                                  0);
        lv_obj_set_style_bg_opa(mark, selected ? LV_OPA_COVER : LV_OPA_50, 0);

        lv_obj_t *name = tinyos_label(row, tinyos_font_family_name(i),
                                      &lv_font_montserrat_10,
                                      selected ? TINYOS_COLOR_TEXT : TINYOS_COLOR_MUTED,
                                      14, 4, 86, 12);
        lv_obj_set_style_text_color(name, lv_color_hex(selected ? TINYOS_COLOR_TEXT
                                                                : TINYOS_COLOR_MUTED),
                                    0);

        lv_obj_t *sample = tinyos_label(row, "Aa 012", tinyos_font_family(i, 12),
                                        selected ? TINYOS_COLOR_CYAN : TINYOS_COLOR_MUTED,
                                        104, 2, 72, 16);
        (void)sample;

        if (selected)
            tinyos_icon(row, LV_SYMBOL_OK, TINYOS_COLOR_CYAN, 188, 5, 10);

        tinyos_focus(row);
        tinyos_shell_add_focus(ctx, row);
        lv_obj_set_user_data(row, (void *)(intptr_t)i);
        lv_obj_add_event_cb(row, font_clicked, LV_EVENT_CLICKED, NULL);
    }
    return parent;
}

const tinyos_module tinyos_module_typeface = {
    .id = "typeface",
    .name = "Typeface",
    .subtitle = "Six families, all sizes",
    .icon = LV_SYMBOL_EDIT,
    .create = typeface_create,
};
