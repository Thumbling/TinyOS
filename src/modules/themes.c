#include "module.h"

#include "../core/settings.h"
#include "../platform.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdio.h>

static int theme_preview;
static bool applying;
static lv_obj_t *preview_card;
static lv_obj_t *preview_title;
static lv_obj_t *preview_a;
static lv_obj_t *preview_b;
static lv_obj_t *sample_a;
static lv_obj_t *sample_b;
static lv_obj_t *sample_bar;
static lv_obj_t *sample_body;
static lv_obj_t *theme_name;
static lv_obj_t *theme_counter;

static void update_preview(void)
{
    const tinyos_theme_definition_t *def = tinyos_theme_definition(theme_preview);

    lv_obj_set_style_bg_color(preview_card, lv_color_hex(def->bg_2), 0);
    lv_obj_set_style_bg_grad_color(preview_card, lv_color_hex(def->bg), 0);
    lv_obj_set_style_bg_grad_dir(preview_card, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_color(preview_card, lv_color_hex(def->line), 0);

    lv_obj_set_style_bg_color(preview_a, lv_color_hex(def->surface), 0);
    lv_obj_set_style_bg_color(preview_b, lv_color_hex(def->surface_2), 0);
    lv_obj_set_style_text_color(preview_title, lv_color_hex(def->muted), 0);
    lv_obj_set_style_text_color(sample_a, lv_color_hex(def->text), 0);
    lv_obj_set_style_text_color(sample_b, lv_color_hex(def->accent), 0);
    lv_obj_set_style_bg_color(sample_bar, lv_color_hex(def->accent_2), 0);
    lv_obj_set_style_text_color(sample_body, lv_color_hex(def->muted), 0);

    lv_label_set_text(theme_name, def->name);
    char counter[24];
    snprintf(counter, sizeof(counter), "%02d / %02d", theme_preview + 1,
             TINYOS_THEME_COUNT);
    lv_label_set_text(theme_counter, counter);
}

static void step_theme(int direction)
{
    theme_preview = (theme_preview + direction + TINYOS_THEME_COUNT) % TINYOS_THEME_COUNT;
    update_preview();
}

static void apply_theme(void)
{
    if (applying)
        return;
    applying = true;

    tinyos_ctx *ctx = tinyos_ctx_get();
    ctx->settings.theme = theme_preview;
    tinyos_settings_save(&ctx->settings, TINYOS_SETTINGS_PATH);
    tinyos_theme_apply(theme_preview);
    tinyos_shell_reload(ctx, "appearance");

    applying = false;
}

static void theme_click_cb(lv_event_t *event)
{
    (void)event;
    apply_theme();
}

static void theme_key_cb(lv_event_t *event)
{
    uint32_t key = lv_event_get_key(event);
    if (key == TINYOS_KEY_PREV) {
        step_theme(-1);
    } else if (key == TINYOS_KEY_NEXT) {
        step_theme(1);
    }
}

static lv_obj_t *themes_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    theme_preview = ctx->settings.theme;
    applying = false;

    tinyos_label(parent, "Themes", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 14, 0, 212, 24);
    tinyos_label(parent, "Live palette preview", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 26, 212, 12);

    preview_card = tinyos_card(parent, 14, 40, 212, 62);
    lv_obj_set_style_border_color(preview_card, lv_color_hex(TINYOS_COLOR_LINE), 0);
    tinyos_focus(preview_card);
    tinyos_shell_add_focus(ctx, preview_card);
    lv_obj_add_event_cb(preview_card, theme_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(preview_card, theme_key_cb, LV_EVENT_KEY, NULL);

    preview_title = tinyos_label(preview_card, "T I N Y O S", &lv_font_montserrat_8,
                                 TINYOS_COLOR_MUTED, 10, 6, 140, 10);

    preview_a = tinyos_obj(preview_card, 10, 18, 102, 36);
    lv_obj_set_style_radius(preview_a, 10, 0);
    sample_a = tinyos_label(preview_a, "Aa", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                            6, 1, 40, 24);
    sample_body = tinyos_label(preview_a, "Body text", &lv_font_montserrat_8,
                               TINYOS_COLOR_MUTED, 6, 24, 88, 10);

    preview_b = tinyos_obj(preview_card, 118, 18, 84, 36);
    lv_obj_set_style_radius(preview_b, 10, 0);
    sample_b = tinyos_label(preview_b, "TinyOS", &lv_font_montserrat_10,
                            TINYOS_COLOR_CYAN, 7, 5, 70, 12);
    sample_bar = tinyos_obj(preview_b, 7, 23, 44, 3);
    lv_obj_set_style_radius(sample_bar, 2, 0);
    lv_obj_set_style_bg_color(sample_bar, lv_color_hex(TINYOS_COLOR_VIOLET), 0);
    lv_obj_set_style_bg_opa(sample_bar, LV_OPA_COVER, 0);

    theme_name = tinyos_label(parent, "", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                              14, 108, 150, 24);
    theme_counter = tinyos_label(parent, "", &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                 166, 112, 60, 14);
    lv_obj_set_style_text_align(theme_counter, LV_TEXT_ALIGN_RIGHT, 0);
    tinyos_label(parent, "X / Y  PREVIEW     A  APPLY", &lv_font_montserrat_8,
                 TINYOS_COLOR_MUTED, 14, 154, 212, 10);

    update_preview();
    return parent;
}

const tinyos_module tinyos_module_themes = {
    .id = "themes",
    .name = "Themes",
    .subtitle = "Twenty-five palettes",
    .icon = LV_SYMBOL_SETTINGS,
    .create = themes_create,
};
