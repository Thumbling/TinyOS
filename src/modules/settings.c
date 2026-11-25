#include "module.h"

#include "../core/settings.h"
#include "../platform.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdio.h>
#include <string.h>

static void persist(tinyos_ctx *ctx)
{
    tinyos_settings_save(&ctx->settings, TINYOS_SETTINGS_PATH);
}

static lv_obj_t *settings_row(tinyos_ctx *ctx, lv_obj_t *list, const char *icon,
                              uint32_t icon_color, const char *text, uint32_t text_color,
                              bool focusable)
{
    lv_obj_t *row = tinyos_obj(list, 0, 0, 212, 22);
    lv_obj_add_style(row, &tinyos_theme.row, 0);
    lv_obj_set_width(row, LV_PCT(100));
    tinyos_icon(row, icon, icon_color, 8, 5, 12);
    tinyos_label(row, text, &lv_font_montserrat_10, text_color, 28, 5, 100, 12);
    if (focusable) {
        tinyos_focus(row);
        tinyos_shell_add_focus(ctx, row);
    }
    return row;
}

static void brightness_changed(lv_event_t *event)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    lv_obj_t *slider = lv_event_get_target(event);
    ctx->settings.brightness = (int)lv_slider_get_value(slider);
    tinyos_brightness_set(ctx->settings.brightness);
    if (lv_event_get_code(event) == LV_EVENT_RELEASED)
        persist(ctx);
}

static void volume_changed(lv_event_t *event)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    lv_obj_t *slider = lv_event_get_target(event);
    ctx->settings.volume = (int)lv_slider_get_value(slider);
    tinyos_audio_set_volume(ctx->audio, ctx->settings.volume);
    if (lv_event_get_code(event) == LV_EVENT_RELEASED)
        persist(ctx);
}

static void clock_changed(lv_event_t *event)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    lv_obj_t *sw = lv_event_get_target(event);
    ctx->settings.clock_24h = lv_obj_has_state(sw, LV_STATE_CHECKED);
    tinyos_shell_refresh_status(ctx);
    persist(ctx);
}

static void performance_clicked(lv_event_t *event)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    lv_obj_t *row = lv_event_get_target(event);
    ctx->settings.performance = (ctx->settings.performance + 1) % 3;
    tinyos_performance_set(ctx->settings.performance);

    lv_obj_t *pill = lv_obj_get_child(row, 2);
    if (pill) {
        lv_obj_t *label = lv_obj_get_child(pill, 0);
        if (label) {
            static const char *names[] = { "Saver", "Balanced", "Turbo" };
            lv_label_set_text(label, names[ctx->settings.performance]);
        }
    }
    persist(ctx);
}

static void about_clicked(lv_event_t *event)
{
    tinyos_shell_show(tinyos_ctx_get(), "about");
}

static void overlay_cancel(lv_event_t *event)
{
    lv_obj_t *overlay = lv_event_get_user_data(event);
    lv_obj_delete(overlay);
}

static void overlay_poweroff(lv_event_t *event)
{
    tinyos_power_off();
}

static lv_obj_t *modal_button(lv_obj_t *parent, int x, int width, const char *text,
                              uint32_t background, uint32_t foreground,
                              lv_event_cb_t callback, void *data)
{
    lv_obj_t *button = tinyos_obj(parent, x, 72, width, 24);
    lv_obj_add_style(button, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(background), 0);
    tinyos_focus(button);
    tinyos_add_click(button, callback, data);
    lv_obj_t *label = tinyos_label(button, text, &lv_font_montserrat_8, foreground,
                                   0, 7, width, 10);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return button;
}

static void confirm_poweroff(tinyos_ctx *ctx)
{
    lv_obj_t *overlay = tinyos_obj(lv_screen_active(), 0, 0, 240, 240);
    lv_obj_set_style_bg_color(overlay, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_70, 0);

    lv_obj_t *card = tinyos_card(overlay, 30, 62, 180, 108);
    tinyos_label(card, "Power off?", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                 14, 12, 152, 20);
    tinyos_label(card, "TinyOS will shut down safely.", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 34, 152, 12);

    lv_obj_t *cancel = modal_button(card, 14, 70, "CANCEL", 0x1B2B36, TINYOS_COLOR_TEXT,
                                    overlay_cancel, overlay);
    lv_obj_t *power = modal_button(card, 92, 74, "POWER OFF", 0x3A1E22,
                                   TINYOS_COLOR_RED, overlay_poweroff, NULL);
    if (ctx->group) {
        lv_group_add_obj(ctx->group, cancel);
        lv_group_add_obj(ctx->group, power);
        lv_group_focus_obj(cancel);
    }
}

static void power_clicked(lv_event_t *event)
{
    confirm_poweroff(tinyos_ctx_get());
}

static lv_obj_t *slider_row(tinyos_ctx *ctx, lv_obj_t *list, const char *icon,
                            uint32_t color, const char *text, int value,
                            lv_event_cb_t callback)
{
    lv_obj_t *row = settings_row(ctx, list, icon, color, text, TINYOS_COLOR_TEXT, false);
    lv_obj_t *slider = lv_slider_create(row);
    lv_obj_set_pos(slider, 130, 3);
    lv_obj_set_size(slider, 72, 16);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x20313D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(color), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(TINYOS_COLOR_TEXT), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 2, LV_PART_KNOB);
    tinyos_focus(slider);
    tinyos_shell_add_focus(ctx, slider);
    lv_obj_add_event_cb(slider, callback, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, callback, LV_EVENT_RELEASED, NULL);
    return row;
}

static lv_obj_t *settings_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    tinyos_label(parent, "Settings", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 14, 0, 212, 24);

    lv_obj_t *list = tinyos_obj(parent, 14, 28, 212, 146);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 2, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollable(list, true);

    slider_row(ctx, list, LV_SYMBOL_EYE_OPEN, TINYOS_COLOR_CYAN, "Brightness",
               ctx->settings.brightness, brightness_changed);
    slider_row(ctx, list, LV_SYMBOL_VOLUME_MAX, TINYOS_COLOR_VIOLET, "Volume",
               ctx->settings.volume, volume_changed);

    lv_obj_t *clock_row = settings_row(ctx, list, LV_SYMBOL_LOOP, TINYOS_COLOR_GREEN,
                                       "24-hour clock", TINYOS_COLOR_TEXT, false);
    lv_obj_t *sw = lv_switch_create(clock_row);
    lv_obj_set_pos(sw, 160, 2);
    lv_obj_set_size(sw, 38, 20);
    if (ctx->settings.clock_24h)
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x20313D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(TINYOS_COLOR_GREEN), LV_PART_INDICATOR);
    tinyos_focus(sw);
    tinyos_shell_add_focus(ctx, sw);
    lv_obj_add_event_cb(sw, clock_changed, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *perf_row = settings_row(ctx, list, LV_SYMBOL_POWER, TINYOS_COLOR_AMBER,
                                      "Performance", TINYOS_COLOR_TEXT, false);
    lv_obj_t *pill = tinyos_obj(perf_row, 126, 2, 72, 18);
    lv_obj_add_style(pill, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(0x22313B), 0);
    static const char *perf_names[] = { "Saver", "Balanced", "Turbo" };
    lv_obj_t *perf_label = tinyos_label(pill, perf_names[ctx->settings.performance],
                                        &lv_font_montserrat_8, TINYOS_COLOR_AMBER,
                                        0, 5, 72, 10);
    lv_obj_set_style_text_align(perf_label, LV_TEXT_ALIGN_CENTER, 0);
    tinyos_focus(perf_row);
    tinyos_shell_add_focus(ctx, perf_row);
    lv_obj_add_event_cb(perf_row, performance_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *about = settings_row(ctx, list, LV_SYMBOL_FILE, TINYOS_COLOR_MUTED,
                                   "About TinyOS", TINYOS_COLOR_TEXT, true);
    tinyos_icon(about, LV_SYMBOL_RIGHT, TINYOS_COLOR_DIM, 194, 6, 10);
    lv_obj_add_event_cb(about, about_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *power = settings_row(ctx, list, LV_SYMBOL_POWER, TINYOS_COLOR_RED,
                                   "Power off", TINYOS_COLOR_RED, true);
    lv_obj_add_event_cb(power, power_clicked, LV_EVENT_CLICKED, NULL);

    return parent;
}

const tinyos_module tinyos_module_settings = {
    .id = "settings",
    .name = "Settings",
    .subtitle = "Tune the experience",
    .icon = LV_SYMBOL_SETTINGS,
    .create = settings_create,
};
