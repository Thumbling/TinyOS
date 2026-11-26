#ifndef TINYOS_CORE_SETTINGS_H
#define TINYOS_CORE_SETTINGS_H

#include <stdbool.h>

#ifndef TINYOS_SETTINGS_PATH
#define TINYOS_SETTINGS_PATH "/data/tinyos/settings.ini"
#endif

enum {
    TINYOS_PERF_SAVER = 0,
    TINYOS_PERF_BALANCED = 1,
    TINYOS_PERF_TURBO = 2,
};

typedef struct {
    int brightness;
    int volume;
    bool clock_24h;
    int performance;
    bool reduce_motion;
    int theme;
    int wallpaper;
    int wallpaper_speed;
    int font_family;
    int text_scale; /* 100, 115 or 130 percent */
} tinyos_settings;

void tinyos_settings_defaults(tinyos_settings *settings);
bool tinyos_settings_load(tinyos_settings *settings, const char *path);
bool tinyos_settings_save(const tinyos_settings *settings, const char *path);

#endif
