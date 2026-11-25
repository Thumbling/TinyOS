#include "../../platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

int tinyos_platform_init(void)
{
    return 0;
}

void tinyos_platform_shutdown(void)
{
}

void tinyos_platform_delay_ms(int milliseconds)
{
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
#else
    struct timespec delay = { milliseconds / 1000, (milliseconds % 1000) * 1000000L };
    nanosleep(&delay, NULL);
#endif
}

static uint32_t host_tick_ms;

uint32_t tinyos_tick_ms(void)
{
    return host_tick_ms;
}

void tinyos_host_tick_advance(uint32_t milliseconds)
{
    host_tick_ms += milliseconds;
}

void tinyos_power_read(tinyos_battery *battery)
{
    if (!battery)
        return;
    battery->percent = 82;
    battery->charging = false;
    battery->present = true;
}

void tinyos_power_off(void)
{
    printf("host: power off requested\n");
    exit(0);
}

void tinyos_power_reboot(void)
{
    printf("host: reboot requested\n");
    exit(0);
}

void tinyos_brightness_set(int percent)
{
    (void)percent;
}

int tinyos_brightness_get(void)
{
    return 78;
}

void tinyos_performance_set(int mode)
{
    (void)mode;
}

int tinyos_performance_get(void)
{
    return 1;
}

void tinyos_stats_read(tinyos_stats *stats)
{
    if (!stats)
        return;
    stats->cpu_mhz = 1008;
    stats->mem_total_kb = 65536;
    stats->mem_available_kb = 38600;
    stats->uptime_s = 8040;
    stats->temp_c = 41;
}
