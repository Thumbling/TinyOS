#ifndef TINYOS_APP_H
#define TINYOS_APP_H

#include <lvgl/lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "core/audio.h"
#include "core/settings.h"

struct tinyos_module;

typedef struct tinyos_ctx {
    tinyos_settings settings;
    tinyos_audio *audio;

    lv_display_t *display;
    lv_indev_t *input;
    lv_group_t *group;
    lv_obj_t *screen;
    lv_obj_t *content;
    lv_obj_t *status_time;
    lv_obj_t *status_battery;
    lv_obj_t *status_brand;
    lv_obj_t *nav_container;
    lv_obj_t *nav[3];

    lv_obj_t *first_focus;
    const struct tinyos_module *current;
    const char *return_to;
    bool running;
} tinyos_ctx;

tinyos_ctx *tinyos_ctx_get(void);
int tinyos_app_init(void);
int tinyos_app_run(void);
void tinyos_app_shutdown(void);

#endif
