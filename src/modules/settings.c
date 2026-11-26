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
    lv_obj_set_style_radius(row, 9, 0);
    tinyos_icon(row, icon, icon_color, 8, 4, 14);
    tinyos_label(row, text, &lv_font_montserrat_12, text_color, 28, 2, 96, 15);
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

static void clock_clicked(lv_event_t *event)
{
    lv_obj_t *sw = lv_event_get_user_data(event);
    if (!sw)
        return;
    if (lv_obj_has_state(sw, LV_STATE_CHECKED))
        lv_obj_remove_state(sw, LV_STATE_CHECKED);
    else
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_send_event(sw, LV_EVENT_VALUE_CHANGED, NULL);
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

static void appearance_clicked(lv_event_t *event)
{
    tinyos_shell_show(tinyos_ctx_get(), "appearance");
}

static lv_group_t *modal_group;
static lv_obj_t *modal_return;

static void overlay_cancel(lv_event_t *event)
{
    lv_obj_t *overlay = lv_event_get_user_data(event);
    lv_obj_delete(overlay);
}

static void overlay_poweroff(lv_event_t *event)
{
    tinyos_power_off();
}

/* B and Start must act on the dialog, not on the screen behind it. Registered
 * before tinyos_focus() so this handler runs ahead of the global key map. */
static void modal_key_cb(lv_event_t *event)
{
    uint32_t key = lv_event_get_key(event);
    if (key != LV_KEY_ESC && key != LV_KEY_HOME)
        return;
    lv_obj_t *overlay = lv_event_get_user_data(event);
    lv_obj_delete(overlay);
    if (key == LV_KEY_HOME)
        tinyos_shell_show(tinyos_ctx_get(), "home");
    lv_event_stop_processing(event);
}

static void modal_restore_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    tinyos_ctx *ctx = tinyos_ctx_get();
    if (modal_group) {
        lv_group_delete(modal_group);
        modal_group = NULL;
    }
    if (ctx->input && ctx->group) {
        lv_indev_set_group(ctx->input, ctx->group);
        if (modal_return && lv_obj_is_valid(modal_return))
            lv_group_focus_obj(modal_return);
    }
    modal_return = NULL;
}

static lv_obj_t *modal_button(lv_obj_t *parent, int x, int width, const char *text,
                              uint32_t background, uint32_t foreground,
                              lv_event_cb_t callback, void *data, lv_obj_t *overlay)
{
    lv_obj_t *button = tinyos_obj(parent, x, 72, width, 24);
    lv_obj_add_style(button, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(background), 0);
    lv_obj_add_event_cb(button, modal_key_cb, LV_EVENT_KEY, overlay);
    tinyos_focus(button);
    tinyos_add_click(button, callback, data);
    lv_obj_t *label = tinyos_label(button, text, &lv_font_montserrat_10, foreground,
                                   0, 6, width, 12);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return button;
}

static void confirm_poweroff(tinyos_ctx *ctx)
{
    modal_return = ctx->group ? lv_group_get_focused(ctx->group) : NULL;

    lv_obj_t *overlay = tinyos_obj(lv_screen_active(), 0, 0, 240, 240);
    lv_obj_set_style_bg_color(overlay, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_70, 0);
    lv_obj_add_event_cb(overlay, modal_restore_cb, LV_EVENT_DELETE, NULL);

    lv_obj_t *card = tinyos_card(overlay, 30, 62, 180, 108);
    tinyos_label(card, "Power off?", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                 14, 12, 152, 20);
    tinyos_label(card, "TinyOS will shut down safely.", &lv_font_montserrat_10,
                 TINYOS_COLOR_MUTED, 14, 34, 152, 12);

    lv_obj_t *cancel = modal_button(card, 14, 70, "CANCEL", TINYOS_COLOR_SURFACE_2,
                                    TINYOS_COLOR_TEXT, overlay_cancel, overlay, overlay);
    lv_obj_t *power = modal_button(card, 92, 74, "POWER OFF",
                                   tinyos_color_mix(TINYOS_COLOR_RED,
                                                    TINYOS_COLOR_BG, 150),
                                   TINYOS_COLOR_TEXT, overlay_poweroff, NULL, overlay);
    if (!modal_group)
        modal_group = lv_group_create();
    lv_group_add_obj(modal_group, cancel);
    lv_group_add_obj(modal_group, power);
    if (ctx->input)
        lv_indev_set_group(ctx->input, modal_group);
    lv_group_focus_obj(cancel);
    tinyos_pop_in(card);
}

static void power_clicked(lv_event_t *event)
{
    confirm_poweroff(tinyos_ctx_get());
}

static void slider_key_cb(lv_event_t *event)
{
    uint32_t key = lv_event_get_key(event);
    if (key != TINYOS_KEY_PREV && key != TINYOS_KEY_NEXT)
        return;

    lv_obj_t *slider = lv_event_get_user_data(event);
    if (!slider)
        slider = lv_event_get_target(event);
    int value = (int)lv_slider_get_value(slider);
    value += key == TINYOS_KEY_PREV ? -5 : 5;
    if (value < 0)
        value = 0;
    else if (value > 100)
        value = 100;
    tinyos_ctx *ctx = tinyos_ctx_get();
    lv_slider_set_value(slider, value,
                        ctx && ctx->settings.reduce_motion ? LV_ANIM_OFF : LV_ANIM_ON);
    lv_obj_send_event(slider, LV_EVENT_VALUE_CHANGED, NULL);
    persist(tinyos_ctx_get());
}

static lv_obj_t *slider_row(tinyos_ctx *ctx, lv_obj_t *list, const char *icon,
                            uint32_t color, const char *text, int value,
                            lv_event_cb_t callback)
{
    lv_obj_t *row = settings_row(ctx, list, icon, color, text, TINYOS_COLOR_TEXT, true);
    lv_obj_t *slider = lv_slider_create(row);
    lv_obj_set_pos(slider, 110, 3);
    lv_obj_set_size(slider, 88, 15);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_hex(TINYOS_COLOR_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(color), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(TINYOS_COLOR_TEXT), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 1, LV_PART_KNOB);
    lv_obj_set_style_anim_duration(slider, 90, 0);
    lv_obj_add_event_cb(row, slider_key_cb, LV_EVENT_KEY, slider);
    lv_obj_add_event_cb(slider, callback, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, callback, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(slider, slider_key_cb, LV_EVENT_KEY, NULL);
    return row;
}

static lv_obj_t *settings_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    tinyos_label(parent, "Settings", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 14, 2, 212, 24);

    lv_obj_t *list = tinyos_obj(parent, 14, 28, 212, 142);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 2, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollable(list, true);

    lv_obj_t *appearance = settings_row(ctx, list, LV_SYMBOL_SETTINGS, TINYOS_COLOR_CYAN,
                                       "Appearance", TINYOS_COLOR_TEXT, true);
    lv_obj_t *theme_pill = tinyos_obj(appearance, 106, 2, 72, 17);
    lv_obj_add_style(theme_pill, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(theme_pill, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    lv_obj_t *theme_label = tinyos_label(theme_pill, tinyos_theme_short_name(ctx->settings.theme),
                                         &lv_font_montserrat_10, TINYOS_COLOR_CYAN,
                                         0, 2, 72, 13);
    lv_obj_set_style_text_align(theme_label, LV_TEXT_ALIGN_CENTER, 0);
    tinyos_icon(appearance, LV_SYMBOL_RIGHT, TINYOS_COLOR_DIM, 186, 5, 12);
    lv_obj_add_event_cb(appearance, appearance_clicked, LV_EVENT_CLICKED, NULL);

    slider_row(ctx, list, LV_SYMBOL_EYE_OPEN, TINYOS_COLOR_CYAN, "Brightness",
               ctx->settings.brightness, brightness_changed);
    slider_row(ctx, list, LV_SYMBOL_VOLUME_MAX, TINYOS_COLOR_VIOLET, "Volume",
               ctx->settings.volume, volume_changed);

    lv_obj_t *clock_row = settings_row(ctx, list, LV_SYMBOL_LOOP, TINYOS_COLOR_GREEN,
                                       "24-hour clock", TINYOS_COLOR_TEXT, true);
    lv_obj_t *sw = lv_switch_create(clock_row);
    lv_obj_set_pos(sw, 164, 3);
    lv_obj_set_size(sw, 34, 15);
    if (ctx->settings.clock_24h)
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(TINYOS_COLOR_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(TINYOS_COLOR_GREEN), LV_PART_INDICATOR);
    tinyos_add_click(clock_row, clock_clicked, sw);
    lv_obj_add_event_cb(sw, clock_changed, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *perf_row = settings_row(ctx, list, LV_SYMBOL_BARS, TINYOS_COLOR_AMBER,
                                      "Performance", TINYOS_COLOR_TEXT, false);
    lv_obj_t *pill = tinyos_obj(perf_row, 106, 2, 72, 17);
    lv_obj_add_style(pill, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    static const char *perf_names[] = { "Saver", "Balanced", "Turbo" };
    lv_obj_t *perf_label = tinyos_label(pill, perf_names[ctx->settings.performance],
                                        &lv_font_montserrat_10, TINYOS_COLOR_AMBER,
                                        0, 2, 72, 13);
    lv_obj_set_style_text_align(perf_label, LV_TEXT_ALIGN_CENTER, 0);
    tinyos_focus(perf_row);
    tinyos_shell_add_focus(ctx, perf_row);
    lv_obj_add_event_cb(perf_row, performance_clicked, LV_EVENT_CLICKED, NULL);

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
