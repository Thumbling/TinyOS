#include "module.h"

#include "../ui/layout.h"
#include "../ui/scale.h"
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
    lv_obj_t *back = tinyos_back_button(parent, back_cb);

    bool large = tinyos_large_text();
    tinyos_page_title(parent, "About", 0);

    lv_obj_t *orb = tinyos_obj(parent, 99, large ? 34 : 22, 42, 42);
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
                                   TINYOS_COLOR_TEXT, TINYOS_CONTENT_X,
                                   large ? 78 : 66, TINYOS_CONTENT_W, 28);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *version = tinyos_label(parent, TINYOS_CODENAME " / v" TINYOS_VERSION,
                                     &lv_font_montserrat_10, TINYOS_COLOR_CYAN,
                                     TINYOS_CONTENT_X, large ? 104 : 92,
                                     TINYOS_CONTENT_W, large ? 18 : 14);
    lv_obj_set_style_text_align(version, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *copy = tinyos_label(parent,
                                  large ? "Modular firmware on\nLinux, U-Boot and LVGL."
                                        : "A modular firmware built on\n"
                                          "mainline Linux, U-Boot and LVGL.",
                                  &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                  TINYOS_CONTENT_X, large ? 122 : 110,
                                  TINYOS_CONTENT_W, large ? 52 : 26);
    lv_obj_set_style_text_align(copy, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(copy, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_line_space(copy, 0, 0);

    if (!large) {
        lv_obj_t *components = tinyos_label(parent,
                                            "Linux 6.12 LTS - U-Boot 2025.10\n"
                                            "Buildroot 2025.02 - LVGL 9.6\n"
                                            "mGBA 0.10.5",
                                            &lv_font_montserrat_8, TINYOS_COLOR_DIM,
                                            TINYOS_CONTENT_X, 136, TINYOS_CONTENT_W, 34);
        lv_obj_set_style_text_align(components, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(components, LV_LABEL_LONG_MODE_WRAP);
    }

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
