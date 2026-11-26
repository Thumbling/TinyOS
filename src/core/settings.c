#include "settings.h"

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tinyos_settings_defaults(tinyos_settings *settings)
{
    if (!settings)
        return;
    settings->brightness = 78;
    settings->volume = 62;
    settings->clock_24h = true;
    settings->performance = TINYOS_PERF_BALANCED;
    settings->reduce_motion = false;
    settings->theme = 0;
    settings->wallpaper = 0;
    settings->wallpaper_speed = 2;
    settings->font_family = 0;
    settings->text_scale = 100;
}

bool tinyos_settings_load(tinyos_settings *settings, const char *path)
{
    if (!settings || !path)
        return false;

    tinyos_settings_defaults(settings);

    char buffer[1024];
    if (tinyos_read_file(path, buffer, sizeof(buffer)) < 0)
        return false;

    for (char *line = strtok(buffer, "\n"); line; line = strtok(NULL, "\n")) {
        line = tinyos_trim(line);
        if (!*line || *line == '#')
            continue;

        char *equals = strchr(line, '=');
        if (!equals)
            continue;
        *equals = '\0';
        char *key = tinyos_trim(line);
        char *value = tinyos_trim(equals + 1);

        if (strcmp(key, "brightness") == 0) {
            settings->brightness = tinyos_clamp_int(atoi(value), 5, 100);
        } else if (strcmp(key, "volume") == 0) {
            settings->volume = tinyos_clamp_int(atoi(value), 0, 100);
        } else if (strcmp(key, "clock_24h") == 0) {
            settings->clock_24h = atoi(value) != 0;
        } else if (strcmp(key, "performance") == 0) {
            settings->performance = tinyos_clamp_int(atoi(value), TINYOS_PERF_SAVER,
                                                     TINYOS_PERF_TURBO);
        } else if (strcmp(key, "reduce_motion") == 0) {
            settings->reduce_motion = atoi(value) != 0;
        } else if (strcmp(key, "theme") == 0) {
            settings->theme = tinyos_clamp_int(atoi(value), 0, 24);
        } else if (strcmp(key, "wallpaper") == 0) {
            settings->wallpaper = tinyos_clamp_int(atoi(value), 0, 9);
        } else if (strcmp(key, "wallpaper_speed") == 0) {
            settings->wallpaper_speed = tinyos_clamp_int(atoi(value), 0, 3);
        } else if (strcmp(key, "font_family") == 0) {
            settings->font_family = tinyos_clamp_int(atoi(value), 0, 5);
        } else if (strcmp(key, "text_scale") == 0) {
            int scale = atoi(value);
            settings->text_scale = scale >= 130 ? 130 : scale >= 115 ? 115 : 100;
        }
    }
    return true;
}

bool tinyos_settings_save(const tinyos_settings *settings, const char *path)
{
    if (!settings || !path)
        return false;

    char parent[512];
    snprintf(parent, sizeof(parent), "%s", path);
    char *slash = strrchr(parent, '/');
    if (slash) {
        *slash = '\0';
        tinyos_mkdir_p(parent);
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer),
             "# TinyOS settings\n"
             "brightness=%d\n"
             "volume=%d\n"
             "clock_24h=%d\n"
             "performance=%d\n"
             "reduce_motion=%d\n"
             "theme=%d\n"
             "wallpaper=%d\n"
             "wallpaper_speed=%d\n"
             "font_family=%d\n"
             "text_scale=%d\n",
             settings->brightness, settings->volume, settings->clock_24h ? 1 : 0,
             settings->performance, settings->reduce_motion ? 1 : 0,
             settings->theme, settings->wallpaper, settings->wallpaper_speed,
             settings->font_family, settings->text_scale);

    return tinyos_write_file_atomic(path, buffer) == 0;
}
