#include "shell.h"

#include "../core/log.h"
#include "../modules/module.h"
#include "../platform.h"
#include "theme.h"
#include "widgets.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static const char *tab_ids[3] = { "home", "apps", "settings" };
static const char *tab_icons[3] = { LV_SYMBOL_HOME, LV_SYMBOL_LIST, LV_SYMBOL_SETTINGS };
static const char *tab_labels[3] = { "Home", "Library", "Settings" };

static bool is_tab(const char *id)
{
    for (int i = 0; i < 3; i++) {
        if (strcmp(id, tab_ids[i]) == 0)
            return true;
    }
    return false;
}

static void nav_click_cb(lv_event_t *event)
{
    const char *id = lv_event_get_user_data(event);
    tinyos_ctx *ctx = tinyos_ctx_get();
    tinyos_shell_show(ctx, id);
}

static void update_nav(tinyos_ctx *ctx, const char *active)
{
    for (int i = 0; i < 3; i++) {
        bool selected = strcmp(active, tab_ids[i]) == 0;
        lv_obj_t *button = ctx->nav[i];
        lv_obj_t *icon = lv_obj_get_child(button, 0);
        if (!icon)
            continue;
        lv_obj_set_style_text_color(icon,
                                    lv_color_hex(selected ? TINYOS_COLOR_CYAN
                                                          : TINYOS_COLOR_MUTED),
                                    0);
        lv_obj_set_style_bg_color(button,
                                  lv_color_hex(selected ? 0x122A36 : TINYOS_COLOR_SURFACE),
                                  0);
        lv_obj_set_style_bg_opa(button, selected ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    }
}

static void screen_key_cb(lv_event_t *event)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    uint32_t key = lv_event_get_key(event);

    if (key == LV_KEY_ESC) {
        tinyos_shell_back(ctx);
    } else if (key == LV_KEY_HOME) {
        tinyos_shell_show(ctx, "home");
    }
}

static void status_timer_cb(lv_timer_t *timer)
{
    tinyos_ctx *ctx = lv_timer_get_user_data(timer);
    tinyos_shell_refresh_status(ctx);
}

static void create_status_bar(tinyos_ctx *ctx)
{
    ctx->status_time = tinyos_label(ctx->screen, "--:--", &lv_font_montserrat_14,
                                    TINYOS_COLOR_TEXT, 14, 7, 58, 18);
    lv_obj_set_style_text_letter_space(ctx->status_time, 0, 0);

    ctx->status_brand = tinyos_label(ctx->screen, "T I N Y O S", &lv_font_montserrat_8,
                                     TINYOS_COLOR_DIM, 72, 10, 96, 12);
    lv_obj_set_style_text_align(ctx->status_brand, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(ctx->status_brand, 1, 0);

    ctx->status_battery = tinyos_label(ctx->screen, "--", &lv_font_montserrat_10,
                                       TINYOS_COLOR_MUTED, 168, 8, 58, 18);
    lv_obj_set_style_text_align(ctx->status_battery, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *line = tinyos_obj(ctx->screen, 14, 26, 212, 1);
    lv_obj_set_style_bg_color(line, lv_color_hex(TINYOS_COLOR_LINE), 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_50, 0);
}

static void create_nav(tinyos_ctx *ctx)
{
    ctx->nav_container = tinyos_card(ctx->screen, 14, 207, 212, 28);
    lv_obj_set_style_radius(ctx->nav_container, 14, 0);

    for (int i = 0; i < 3; i++) {
        lv_obj_t *button = tinyos_obj(ctx->nav_container, 8 + i * 68, 3, 60, 22);
        lv_obj_set_style_radius(button, 11, 0);
        lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
        tinyos_focus(button);
        lv_obj_add_event_cb(button, nav_click_cb, LV_EVENT_CLICKED, (void *)tab_ids[i]);

        lv_obj_t *icon = lv_label_create(button);
        lv_label_set_text(icon, tab_icons[i]);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(icon, lv_color_hex(TINYOS_COLOR_MUTED), 0);
        lv_obj_center(icon);

        lv_obj_set_style_text_letter_space(button, 0, 0);
        lv_obj_set_user_data(button, (void *)tab_labels[i]);
        ctx->nav[i] = button;
    }
}

void tinyos_shell_add_focus(tinyos_ctx *ctx, lv_obj_t *obj)
{
    if (!ctx || !obj || !ctx->group)
        return;
    lv_group_add_obj(ctx->group, obj);
    if (!ctx->first_focus)
        ctx->first_focus = obj;
}

void tinyos_shell_show(tinyos_ctx *ctx, const char *id)
{
    if (!ctx || !id)
        return;

    const tinyos_module *module = tinyos_module_find(id);
    if (!module) {
        TINYOS_LOGW("unknown module '%s'", id);
        return;
    }

    if (ctx->current && is_tab(ctx->current->id) && !is_tab(module->id))
        ctx->return_to = ctx->current->id;
    if (is_tab(module->id))
        ctx->return_to = NULL;

    lv_obj_clean(ctx->content);

    if (ctx->group) {
        for (int i = 0; i < 3; i++) {
            if (ctx->nav[i])
                lv_group_remove_obj(ctx->nav[i]);
        }
        lv_group_delete(ctx->group);
        ctx->group = NULL;
    }

    ctx->group = lv_group_create();
    ctx->first_focus = NULL;
    for (int i = 0; i < 3; i++) {
        if (ctx->nav[i])
            lv_group_add_obj(ctx->group, ctx->nav[i]);
    }
    if (ctx->input)
        lv_indev_set_group(ctx->input, ctx->group);

    module->create(ctx, ctx->content);

    lv_group_focus_obj(ctx->first_focus ? ctx->first_focus : ctx->nav[0]);
    update_nav(ctx, module->id);
    tinyos_content_in(ctx->content);
    ctx->current = module;
    tinyos_shell_refresh_status(ctx);
}

void tinyos_shell_back(tinyos_ctx *ctx)
{
    if (!ctx)
        return;
    if (ctx->return_to)
        tinyos_shell_show(ctx, ctx->return_to);
    else if (ctx->current && strcmp(ctx->current->id, "home") != 0)
        tinyos_shell_show(ctx, "home");
}

void tinyos_shell_refresh_status(tinyos_ctx *ctx)
{
    if (!ctx)
        return;

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    struct tm value;
    if (local)
        value = *local;
    else
        memset(&value, 0, sizeof(value));

    char time_text[24];
    if (ctx->settings.clock_24h) {
        strftime(time_text, sizeof(time_text), "%H:%M", &value);
    } else {
        strftime(time_text, sizeof(time_text), "%I:%M", &value);
        if (time_text[0] == '0')
            memmove(time_text, time_text + 1, strlen(time_text));
    }
    lv_label_set_text(ctx->status_time, time_text);

    tinyos_battery battery;
    tinyos_power_read(&battery);
    if (!battery.present || battery.percent < 0) {
        lv_label_set_text(ctx->status_battery, "--");
        return;
    }

    const char *icon = LV_SYMBOL_BATTERY_EMPTY;
    if (battery.percent >= 80)
        icon = LV_SYMBOL_BATTERY_FULL;
    else if (battery.percent >= 55)
        icon = LV_SYMBOL_BATTERY_3;
    else if (battery.percent >= 30)
        icon = LV_SYMBOL_BATTERY_2;
    else if (battery.percent >= 10)
        icon = LV_SYMBOL_BATTERY_1;

    char battery_text[32];
    snprintf(battery_text, sizeof(battery_text), "%s %d%%",
             battery.charging ? LV_SYMBOL_CHARGE : icon, battery.percent);
    lv_label_set_text(ctx->status_battery, battery_text);
}

void tinyos_shell_init(tinyos_ctx *ctx)
{
    if (!ctx || !ctx->display)
        return;

    ctx->screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(ctx->screen);
    tinyos_gradient(ctx->screen, TINYOS_COLOR_BG_2, TINYOS_COLOR_BG);
    lv_obj_set_style_bg_opa(ctx->screen, LV_OPA_COVER, 0);
    lv_screen_load(ctx->screen);

    lv_obj_t *glow_cyan = tinyos_obj(ctx->screen, -70, -80, 200, 200);
    lv_obj_set_style_radius(glow_cyan, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(glow_cyan, lv_color_hex(TINYOS_COLOR_CYAN), 0);
    lv_obj_set_style_bg_opa(glow_cyan, LV_OPA_10, 0);

    lv_obj_t *glow_violet = tinyos_obj(ctx->screen, 150, 140, 170, 170);
    lv_obj_set_style_radius(glow_violet, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(glow_violet, lv_color_hex(TINYOS_COLOR_VIOLET), 0);
    lv_obj_set_style_bg_opa(glow_violet, LV_OPA_10, 0);

    create_status_bar(ctx);

    ctx->content = tinyos_obj(ctx->screen, 0, 30, 240, 174);
    lv_obj_set_style_bg_opa(ctx->content, LV_OPA_TRANSP, 0);

    create_nav(ctx);

    lv_obj_add_event_cb(ctx->screen, screen_key_cb, LV_EVENT_KEY, ctx);
    lv_timer_create(status_timer_cb, 1000, ctx);

    tinyos_shell_show(ctx, "home");
}
