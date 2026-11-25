#ifndef TINYOS_PLATFORM_H
#define TINYOS_PLATFORM_H

#include <lvgl/lvgl.h>
#include <stdbool.h>
#include <stdint.h>

/* Semantic shoulder/face keys, outside LVGL's keypad navigation range. */
#define TINYOS_KEY_PREV 0xE000u
#define TINYOS_KEY_NEXT 0xE001u

typedef struct {
    int percent;
    bool charging;
    bool present;
} tinyos_battery;

typedef struct {
    int cpu_mhz;
    int mem_total_kb;
    int mem_available_kb;
    long uptime_s;
    int temp_c;
} tinyos_stats;

int tinyos_platform_init(void);
void tinyos_platform_shutdown(void);
void tinyos_platform_delay_ms(int milliseconds);
uint32_t tinyos_tick_ms(void);

lv_display_t *tinyos_display_create(void);
lv_indev_t *tinyos_input_create(void);

void tinyos_power_read(tinyos_battery *battery);
void tinyos_power_off(void);
void tinyos_power_reboot(void);

void tinyos_brightness_set(int percent);
int tinyos_brightness_get(void);
void tinyos_performance_set(int mode);
int tinyos_performance_get(void);
void tinyos_stats_read(tinyos_stats *stats);

#ifdef TINYOS_SIM
const uint16_t *tinyos_host_framebuffer(void);
void tinyos_host_input_key(uint32_t key);
void tinyos_host_tick_advance(uint32_t milliseconds);
#endif

#endif
