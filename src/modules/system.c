#include "module.h"

#include "../core/log.h"
#include "../platform.h"
#include "../ui/layout.h"
#include "../ui/scale.h"
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
    bool large = tinyos_large_text();
    lv_obj_t *card = tinyos_card(parent, x, y, TINYOS_GRID_CARD_W,
                                 large ? 60 : 56);
    tinyos_gradient(card, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    lv_obj_t *label = tinyos_label(card, title, &lv_font_montserrat_10, TINYOS_COLOR_DIM,
                                   8, 5, 86, 13);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    /* Pick the base that lands on the 16 px cut at every scale, so
     * "1008 MHz" and "26/64 MB" stay on one line inside the 102 px card. */
    int scale = tinyos_text_scale();
    const lv_font_t *value_base = scale >= 130 ? &lv_font_montserrat_12
                                  : scale >= 115 ? &lv_font_montserrat_14
                                                 : &lv_font_montserrat_16;
    *value = tinyos_label(card, "-", value_base, TINYOS_COLOR_TEXT,
                          8, large ? 29 : 19, 86, large ? 24 : 20);
    lv_obj_set_style_text_align(*value, LV_TEXT_ALIGN_CENTER, 0);
    *sub = tinyos_label(card, "", &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                        8, 40, 86, 12);
    lv_obj_set_style_text_align(*sub, LV_TEXT_ALIGN_CENTER, 0);
    if (large) {
        /* No secondary note at large sizes, so centre the label/value pair
         * in the taller 60 px card instead of leaving 14 px dead space. */
        lv_obj_set_pos(label, 8, 8);
        lv_obj_set_pos(*value, 8, 29);
        lv_obj_set_size(*value, 86, 24);
        lv_obj_set_hidden(*sub, true);
    }
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
    snprintf(buffer, sizeof(buffer), "%d C", stats.temp_c);
    lv_label_set_text(view->cpu_sub, buffer);

    int used_mb = (stats.mem_total_kb - stats.mem_available_kb) / 1024;
    int total_mb = stats.mem_total_kb / 1024;
    /* Compact at every scale: it keeps the centred value inside the 86 px
     * text box with real side bearing, including at 100 %. */
    snprintf(buffer, sizeof(buffer), "%d/%d MB", used_mb, total_mb);
    lv_label_set_text(view->mem_value, buffer);
    snprintf(buffer, sizeof(buffer), "%d MB free", stats.mem_available_kb / 1024);
    lv_label_set_text(view->mem_sub, buffer);

    if (battery.present && battery.percent >= 0) {
        snprintf(buffer, sizeof(buffer), "%d%%", battery.percent);
        lv_label_set_text(view->battery_value, buffer);
        lv_label_set_text(view->battery_sub, battery.charging ? "Charging" : "On battery");
    } else {
        lv_label_set_text(view->battery_value, "-");
        lv_label_set_text(view->battery_sub, "No battery");
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

    lv_obj_t *back = tinyos_back_button(parent, back_cb);

    bool large = tinyos_large_text();
    tinyos_page_title(parent, "System", 0);

    int top = 40;
    int bottom = large ? 108 : 104;
    stat_card(parent, TINYOS_CONTENT_X, top, "CPU", &view->cpu_value, &view->cpu_sub);
    stat_card(parent, TINYOS_GRID_CARD_X2, top, "MEMORY", &view->mem_value, &view->mem_sub);
    stat_card(parent, TINYOS_CONTENT_X, bottom, "BATTERY", &view->battery_value, &view->battery_sub);
    stat_card(parent, TINYOS_GRID_CARD_X2, bottom, "UPTIME", &view->uptime_value, &view->uptime_sub);

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
    .icon = LV_SYMBOL_CHARGE,
    .create = system_create,
};
