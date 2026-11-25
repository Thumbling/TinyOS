/*
 * TinyOS Linux power, brightness, performance and telemetry backend.
 *
 * Everything here is sysfs/procfs based so it works with the mainline
 * AXP209, pwm-backlight, cpufreq and thermal drivers without private ioctls.
 */
#include "../../platform.h"
#include "../../core/log.h"
#include "../../core/settings.h"
#include "../../core/util.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <time.h>
#include <unistd.h>

static char backlight_path[96];
static bool backlight_resolved;
static char governor_path[96] = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";

static int read_int_file(const char *path)
{
    char buffer[64];
    if (tinyos_read_file(path, buffer, sizeof(buffer)) < 0)
        return -1;
    char *value = tinyos_trim(buffer);
    if (!*value)
        return -1;
    return atoi(value);
}

static bool write_text_file(const char *path, const char *value)
{
    FILE *file = fopen(path, "w");
    if (!file)
        return false;
    bool ok = fprintf(file, "%s\n", value) > 0;
    if (fclose(file) != 0)
        ok = false;
    return ok;
}

static void resolve_backlight(void)
{
    if (backlight_resolved)
        return;
    backlight_resolved = true;

    const char *candidates[] = {
        "/sys/class/backlight/backlight",
        "/sys/class/backlight/pwm-backlight",
    };
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        char probe[128];
        snprintf(probe, sizeof(probe), "%s/max_brightness", candidates[i]);
        if (access(probe, R_OK) == 0) {
            snprintf(backlight_path, sizeof(backlight_path), "%s", candidates[i]);
            return;
        }
    }

    DIR *dir = opendir("/sys/class/backlight");
    if (!dir)
        return;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        char probe[160];
        snprintf(probe, sizeof(probe), "/sys/class/backlight/%s/max_brightness", entry->d_name);
        if (access(probe, R_OK) == 0) {
            snprintf(backlight_path, sizeof(backlight_path), "/sys/class/backlight/%s",
                     entry->d_name);
            break;
        }
    }
    closedir(dir);
}

int tinyos_platform_init(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    TINYOS_LOGI("TinyOS platform init");
    resolve_backlight();
    return 0;
}

void tinyos_platform_shutdown(void)
{
    sync();
}

void tinyos_platform_delay_ms(int milliseconds)
{
    if (milliseconds <= 0)
        return;
    struct timespec request = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (long)(milliseconds % 1000) * 1000000L,
    };
    while (nanosleep(&request, &request) != 0 && errno == EINTR)
        ;
}

uint32_t tinyos_tick_ms(void)
{
    static struct timespec start;
    static bool started;
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    if (!started) {
        start = now;
        started = true;
    }
    uint64_t seconds = (uint64_t)(now.tv_sec - start.tv_sec);
    int64_t nanoseconds = (int64_t)now.tv_nsec - (int64_t)start.tv_nsec;
    return (uint32_t)(seconds * 1000 + (uint64_t)(nanoseconds / 1000000));
}

void tinyos_power_read(tinyos_battery *battery)
{
    if (!battery)
        return;
    battery->percent = 0;
    battery->charging = false;
    battery->present = false;

    DIR *dir = opendir("/sys/class/power_supply");
    if (!dir)
        return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        char path[160];
        char type[32];
        snprintf(path, sizeof(path), "/sys/class/power_supply/%s/type", entry->d_name);
        if (tinyos_read_file(path, type, sizeof(type)) < 0)
            continue;
        if (strncmp(tinyos_trim(type), "Battery", 7) != 0)
            continue;

        snprintf(path, sizeof(path), "/sys/class/power_supply/%s/capacity", entry->d_name);
        int capacity = read_int_file(path);
        if (capacity < 0)
            capacity = 0;
        battery->percent = tinyos_clamp_int(capacity, 0, 100);
        battery->present = true;

        char status[32];
        snprintf(path, sizeof(path), "/sys/class/power_supply/%s/status", entry->d_name);
        if (tinyos_read_file(path, status, sizeof(status)) >= 0) {
            char *trimmed = tinyos_trim(status);
            battery->charging = strstr(trimmed, "Charging") != NULL ||
                                strstr(trimmed, "Full") != NULL;
        }
        break;
    }
    closedir(dir);
}

void tinyos_brightness_set(int percent)
{
    resolve_backlight();
    if (!backlight_path[0])
        return;

    char path[128];
    snprintf(path, sizeof(path), "%s/max_brightness", backlight_path);
    int maximum = read_int_file(path);
    if (maximum <= 0)
        return;

    int value = tinyos_clamp_int(percent, 0, 100) * maximum / 100;

    char value_text[16];
    snprintf(value_text, sizeof(value_text), "%d", value);
    snprintf(path, sizeof(path), "%s/brightness", backlight_path);
    if (!write_text_file(path, value_text))
        TINYOS_LOGW("cannot set brightness via %s", path);
}

int tinyos_brightness_get(void)
{
    resolve_backlight();
    if (!backlight_path[0])
        return 100;

    char path[128];
    snprintf(path, sizeof(path), "%s/max_brightness", backlight_path);
    int maximum = read_int_file(path);
    snprintf(path, sizeof(path), "%s/brightness", backlight_path);
    int value = read_int_file(path);
    if (maximum <= 0 || value < 0)
        return 100;
    return tinyos_clamp_int(value * 100 / maximum, 0, 100);
}

void tinyos_performance_set(int mode)
{
    const char *governor;
    switch (mode) {
    case TINYOS_PERF_SAVER:
        governor = "powersave";
        break;
    case TINYOS_PERF_TURBO:
        governor = "performance";
        break;
    default:
        governor = "schedutil";
        break;
    }

    if (!write_text_file(governor_path, governor)) {
        /* Older kernels may not expose schedutil; ondemand is the next best. */
        if (mode == TINYOS_PERF_BALANCED)
            write_text_file(governor_path, "ondemand");
        else
            TINYOS_LOGW("cannot set cpufreq governor %s", governor);
    }
}

int tinyos_performance_get(void)
{
    char governor[32];
    if (tinyos_read_file(governor_path, governor, sizeof(governor)) < 0)
        return TINYOS_PERF_BALANCED;

    char *value = tinyos_trim(governor);
    if (strncmp(value, "performance", 11) == 0)
        return TINYOS_PERF_TURBO;
    if (strncmp(value, "powersave", 9) == 0)
        return TINYOS_PERF_SAVER;
    return TINYOS_PERF_BALANCED;
}

void tinyos_stats_read(tinyos_stats *stats)
{
    if (!stats)
        return;
    memset(stats, 0, sizeof(*stats));

    char buffer[512];
    if (tinyos_read_file("/proc/meminfo", buffer, sizeof(buffer)) >= 0) {
        char *line = strtok(buffer, "\n");
        while (line) {
            int value;
            if (sscanf(line, "MemTotal: %d kB", &value) == 1)
                stats->mem_total_kb = value;
            else if (sscanf(line, "MemAvailable: %d kB", &value) == 1)
                stats->mem_available_kb = value;
            line = strtok(NULL, "\n");
        }
    }

    if (tinyos_read_file("/proc/uptime", buffer, sizeof(buffer)) >= 0)
        stats->uptime_s = (long)strtod(buffer, NULL);

    int frequency = read_int_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    stats->cpu_mhz = frequency > 0 ? frequency / 1000 : 1008;

    int temperature = read_int_file("/sys/class/thermal/thermal_zone0/temp");
    stats->temp_c = temperature > 0 ? temperature / 1000 : 0;
}

void tinyos_power_off(void)
{
    TINYOS_LOGI("powering off");
    sync();
    reboot(RB_POWER_OFF);
    _exit(0);
}

void tinyos_power_reboot(void)
{
    TINYOS_LOGI("rebooting");
    sync();
    reboot(RB_AUTOBOOT);
    _exit(0);
}
