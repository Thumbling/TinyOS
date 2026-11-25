#ifndef TINYOS_WALLPAPER_H
#define TINYOS_WALLPAPER_H

#include <lvgl/lvgl.h>
#include <stdbool.h>

#define TINYOS_WALLPAPER_COUNT 10
#define TINYOS_WALLPAPER_SPEEDS 4

typedef struct tinyos_wallpaper tinyos_wallpaper;

tinyos_wallpaper *tinyos_wallpaper_create(lv_obj_t *parent, int width, int height,
                                          int style, int speed, bool preview);
void tinyos_wallpaper_destroy(tinyos_wallpaper *wallpaper);
const char *tinyos_wallpaper_name(int style);
const char *tinyos_wallpaper_speed_name(int speed);
int tinyos_wallpaper_clamp_style(int style);
int tinyos_wallpaper_clamp_speed(int speed);

#endif
