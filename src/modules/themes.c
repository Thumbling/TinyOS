#include "module.h"

#include "../core/settings.h"
#include "../platform.h"
#include "../ui/scale.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdio.h>

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

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
    /* Slide the samples inside the fixed card, not the card itself: the
     * 14 px grid stays intact while the preview steps. */
    tinyos_nudge(preview_a, direction > 0 ? 5 : -5);
    tinyos_nudge(preview_b, direction > 0 ? 5 : -5);
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
    bool large = tinyos_large_text();

    lv_obj_t *back = tinyos_obj(parent, 10, 2, 26, 26);
    lv_obj_add_style(back, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(back, 10, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    tinyos_focus(back);
    tinyos_add_click(back, back_cb, NULL);
    tinyos_icon(back, LV_SYMBOL_LEFT, TINYOS_COLOR_MUTED, 0, 0, 26);
    lv_obj_center(lv_obj_get_child(back, 0));
    tinyos_label(parent, "Themes", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 44, 2, 170, 24);
    if (!large)
        tinyos_label(parent, "Live palette preview", &lv_font_montserrat_10,
                     TINYOS_COLOR_MUTED, 44, 25, 180, 12);

    preview_card = tinyos_card(parent, 14, large ? 38 : 42, 212, large ? 74 : 62);
    lv_obj_set_style_border_color(preview_card, lv_color_hex(TINYOS_COLOR_LINE), 0);
    tinyos_focus(preview_card);
    tinyos_shell_add_focus(ctx, preview_card);
    lv_obj_add_event_cb(preview_card, theme_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(preview_card, theme_key_cb, LV_EVENT_KEY, NULL);

    preview_title = tinyos_label(preview_card, "PALETTE PREVIEW", &lv_font_montserrat_10,
                                 TINYOS_COLOR_MUTED, 10, large ? 2 : 4, 140,
                                 large ? 16 : 12);

    preview_a = tinyos_obj(preview_card, 10, large ? 20 : 18, 102, large ? 50 : 36);
    lv_obj_set_style_radius(preview_a, 10, 0);
    sample_a = tinyos_label(preview_a, "Aa", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                            6, large ? 0 : 1, 40, large ? 28 : 24);
    sample_body = tinyos_label(preview_a, "Body text", &lv_font_montserrat_10,
                               TINYOS_COLOR_MUTED, 6, large ? 26 : 24, 88,
                               large ? 18 : 10);

    preview_b = tinyos_obj(preview_card, 118, large ? 20 : 18, 84, large ? 50 : 36);
    lv_obj_set_style_radius(preview_b, 10, 0);
    sample_b = tinyos_label(preview_b, "TinyOS", &lv_font_montserrat_10,
                            TINYOS_COLOR_CYAN, 7, large ? 4 : 5, 70,
                            large ? 16 : 12);
    sample_bar = tinyos_obj(preview_b, 7, large ? 30 : 23, 44, 3);
    lv_obj_set_style_radius(sample_bar, 2, 0);
    lv_obj_set_style_bg_color(sample_bar, lv_color_hex(TINYOS_COLOR_VIOLET), 0);
    lv_obj_set_style_bg_opa(sample_bar, LV_OPA_COVER, 0);

    theme_name = tinyos_label(parent, "", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                              14, large ? 116 : 110, 150, large ? 26 : 24);
    theme_counter = tinyos_label(parent, "", &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                 166, large ? 118 : 114, 60, 14);
    lv_obj_set_style_text_align(theme_counter, LV_TEXT_ALIGN_RIGHT, 0);
    tinyos_label(parent, "X / Y switch     A apply", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 152, 212, 13);

    update_preview();
    tinyos_shell_add_focus(ctx, back);
    return parent;
}

const tinyos_module tinyos_module_themes = {
    .id = "themes",
    .name = "Themes",
    .subtitle = "Twenty-five palettes",
    .icon = LV_SYMBOL_SETTINGS,
    .create = themes_create,
};
