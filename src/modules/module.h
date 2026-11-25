#ifndef TINYOS_MODULES_MODULE_H
#define TINYOS_MODULES_MODULE_H

#include "../app.h"

typedef struct tinyos_module {
    const char *id;
    const char *name;
    const char *subtitle;
    const char *icon;
    lv_obj_t *(*create)(tinyos_ctx *ctx, lv_obj_t *parent);
} tinyos_module;

extern const tinyos_module tinyos_module_home;
extern const tinyos_module tinyos_module_apps;
extern const tinyos_module tinyos_module_music;
extern const tinyos_module tinyos_module_settings;
extern const tinyos_module tinyos_module_system;
extern const tinyos_module tinyos_module_about;

const tinyos_module *tinyos_module_find(const char *id);

#endif
