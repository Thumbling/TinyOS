#include "module.h"

#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"
#include "../version.h"

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

static lv_obj_t *about_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    lv_obj_t *back = tinyos_obj(parent, 10, 2, 26, 26);
    lv_obj_add_style(back, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(back, 10, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    tinyos_focus(back);
    tinyos_add_click(back, back_cb, NULL);
    tinyos_icon(back, LV_SYMBOL_LEFT, TINYOS_COLOR_MUTED, 0, 0, 26);
    lv_obj_center(lv_obj_get_child(back, 0));

    tinyos_label(parent, "About", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 44, 2, 150, 24);

    lv_obj_t *orb = tinyos_obj(parent, 99, 22, 42, 42);
    lv_obj_set_style_radius(orb, LV_RADIUS_CIRCLE, 0);
    tinyos_gradient(orb, TINYOS_COLOR_CYAN, TINYOS_COLOR_VIOLET);
    lv_obj_set_style_shadow_width(orb, 18, 0);
    lv_obj_set_style_shadow_color(orb, lv_color_hex(TINYOS_COLOR_CYAN), 0);
    lv_obj_set_style_shadow_opa(orb, LV_OPA_30, 0);
    lv_obj_t *inner = tinyos_obj(orb, 4, 4, 34, 34);
    lv_obj_set_style_radius(inner, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(inner, lv_color_hex(TINYOS_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(inner, LV_OPA_COVER, 0);
    tinyos_label(inner, "T", &lv_font_montserrat_20, TINYOS_COLOR_TEXT, 0, 5, 34, 24);
    lv_obj_set_style_text_align(lv_obj_get_child(inner, 0), LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *title = tinyos_label(parent, "TinyOS", &lv_font_montserrat_24,
                                   TINYOS_COLOR_TEXT, 14, 66, 212, 26);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *version = tinyos_label(parent, TINYOS_CODENAME " / v" TINYOS_VERSION,
                                     &lv_font_montserrat_10, TINYOS_COLOR_CYAN,
                                     14, 92, 212, 14);
    lv_obj_set_style_text_align(version, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *copy = tinyos_label(parent,
                                  "A modular firmware built on\n"
                                  "mainline Linux, U-Boot and LVGL.",
                                  &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                  20, 110, 200, 26);
    lv_obj_set_style_text_align(copy, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(copy, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_line_space(copy, 0, 0);

    lv_obj_t *components = tinyos_label(parent,
                                        "Linux 6.12 LTS - U-Boot 2025.10\n"
                                        "Buildroot 2025.02 - LVGL 9.6",
                                        &lv_font_montserrat_10, TINYOS_COLOR_DIM,
                                        10, 140, 220, 26);
    lv_obj_set_style_text_align(components, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(components, LV_LABEL_LONG_MODE_WRAP);

    tinyos_shell_add_focus(ctx, back);
    tinyos_reveal(orb, 0);
    tinyos_reveal(title, 40);
    tinyos_reveal(copy, 80);
    return parent;
}

const tinyos_module tinyos_module_about = {
    .id = "about",
    .name = "About",
    .subtitle = "Version and upstream credits",
    .icon = LV_SYMBOL_FILE,
    .create = about_create,
};
