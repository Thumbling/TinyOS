#include "module.h"

#include "../core/log.h"
#include "../platform.h"
#include "../ui/shell.h"
#include "../version.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    tinyos_ctx *ctx;
    lv_obj_t *cpu_value;
    lv_obj_t *cpu_sub;
    lv_obj_t *mem_value;
    lv_obj_t *mem_sub;
    lv_obj_t *battery_value;
    lv_obj_t *battery_sub;
    lv_obj_t *uptime_value;
    lv_obj_t *uptime_sub;
    lv_timer_t *timer;
} system_view;

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

static lv_obj_t *stat_card(lv_obj_t *parent, int x, int y, const char *title,
                           lv_obj_t **value, lv_obj_t **sub)
{
    lv_obj_t *card = tinyos_card(parent, x, y, 102, 54);
    tinyos_gradient(card, 0x13212C, 0x0E171F);
    tinyos_label(card, title, &lv_font_montserrat_8, TINYOS_COLOR_DIM, 8, 6, 86, 10);
    *value = tinyos_label(card, "-", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                          8, 18, 86, 20);
    *sub = tinyos_label(card, "", &lv_font_montserrat_8, TINYOS_COLOR_MUTED,
                        8, 39, 86, 10);
    return card;
}

static void system_refresh(system_view *view)
{
    tinyos_stats stats;
    tinyos_battery battery;
    tinyos_stats_read(&stats);
    tinyos_power_read(&battery);

    char buffer[48];

    snprintf(buffer, sizeof(buffer), "%d MHz", stats.cpu_mhz > 0 ? stats.cpu_mhz : 0);
    lv_label_set_text(view->cpu_value, buffer);
    snprintf(buffer, sizeof(buffer), "%d C - Cortex-A7", stats.temp_c);
    lv_label_set_text(view->cpu_sub, buffer);

    int used_mb = (stats.mem_total_kb - stats.mem_available_kb) / 1024;
    int total_mb = stats.mem_total_kb / 1024;
    snprintf(buffer, sizeof(buffer), "%d / %d MB", used_mb, total_mb);
    lv_label_set_text(view->mem_value, buffer);
    snprintf(buffer, sizeof(buffer), "%d MB available", stats.mem_available_kb / 1024);
    lv_label_set_text(view->mem_sub, buffer);

    if (battery.present && battery.percent >= 0) {
        snprintf(buffer, sizeof(buffer), "%d%%", battery.percent);
        lv_label_set_text(view->battery_value, buffer);
        lv_label_set_text(view->battery_sub, battery.charging ? "Charging" : "On battery");
    } else {
        lv_label_set_text(view->battery_value, "-");
        lv_label_set_text(view->battery_sub, "No battery data");
    }

    long hours = stats.uptime_s / 3600;
    long minutes = (stats.uptime_s % 3600) / 60;
    snprintf(buffer, sizeof(buffer), "%ldh %02ldm", hours, minutes);
    lv_label_set_text(view->uptime_value, buffer);
    lv_label_set_text(view->uptime_sub, "Since boot");
}

static void system_timer_cb(lv_timer_t *timer)
{
    system_refresh(lv_timer_get_user_data(timer));
}

static void system_delete_cb(lv_event_t *event)
{
    system_view *view = lv_event_get_user_data(event);
    if (!view)
        return;
    if (view->timer)
        lv_timer_delete(view->timer);
    free(view);
}

static lv_obj_t *system_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    system_view *view = calloc(1, sizeof(*view));
    if (!view)
        return parent;
    view->ctx = ctx;

    lv_obj_t *back = tinyos_obj(parent, 10, 2, 26, 26);
    lv_obj_add_style(back, &tinyos_theme.pill, 0);
    lv_obj_set_style_radius(back, 10, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(0x14232E), 0);
    tinyos_focus(back);
    tinyos_add_click(back, back_cb, NULL);
    tinyos_icon(back, LV_SYMBOL_LEFT, TINYOS_COLOR_MUTED, 0, 0, 26);
    lv_obj_t *back_icon = lv_obj_get_child(back, 0);
    lv_obj_center(back_icon);

    tinyos_label(parent, "System", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 44, 2, 150, 24);
    tinyos_label(parent, "Live hardware.", &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                 45, 24, 170, 12);

    stat_card(parent, 14, 44, "CPU", &view->cpu_value, &view->cpu_sub);
    stat_card(parent, 124, 44, "MEMORY", &view->mem_value, &view->mem_sub);
    stat_card(parent, 14, 104, "BATTERY", &view->battery_value, &view->battery_sub);
    stat_card(parent, 124, 104, "UPTIME", &view->uptime_value, &view->uptime_sub);

    tinyos_label(parent, "TinyOS " TINYOS_VERSION " - Linux 6.12 LTS",
                 &lv_font_montserrat_8, TINYOS_COLOR_DIM, 14, 164, 212, 10);

    tinyos_shell_add_focus(ctx, back);
    view->timer = lv_timer_create(system_timer_cb, 1000, view);
    lv_obj_add_event_cb(parent, system_delete_cb, LV_EVENT_DELETE, view);
    system_refresh(view);

    tinyos_reveal(view->cpu_value, 40);
    tinyos_reveal(view->mem_value, 70);
    tinyos_reveal(view->battery_value, 100);
    tinyos_reveal(view->uptime_value, 130);
    return parent;
}

const tinyos_module tinyos_module_system = {
    .id = "system",
    .name = "System",
    .subtitle = "Live hardware stats",
    .icon = LV_SYMBOL_DRIVE,
    .create = system_create,
};
