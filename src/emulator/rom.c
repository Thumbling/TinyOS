#include "rom.h"

#include "../core/log.h"

#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct {
    const char *directory;
    const char *extension;
    tinyos_rom_system system;
} rom_directory;

static const rom_directory directories[] = {
    { TINYOS_ROMS_ROOT "/gb", ".gb", TINYOS_ROM_GB },
    { TINYOS_ROMS_ROOT "/gbc", ".gbc", TINYOS_ROM_GBC },
    { TINYOS_ROMS_ROOT "/gba", ".gba", TINYOS_ROM_GBA },
};

static bool has_extension(const char *name, const char *extension)
{
    size_t name_length = strlen(name);
    size_t extension_length = strlen(extension);
    if (name_length <= extension_length)
        return false;
    return strcasecmp(name + name_length - extension_length, extension) == 0;
}

static void display_name(char *destination, size_t size, const char *file_name,
                         const char *extension)
{
    if (!destination || size == 0)
        return;

    size_t length = strlen(file_name);
    size_t extension_length = strlen(extension);
    if (length > extension_length)
        length -= extension_length;
    if (length >= size)
        length = size - 1;

    memcpy(destination, file_name, length);
    destination[length] = '\0';
    for (char *p = destination; *p; p++) {
        if (*p == '_')
            *p = ' ';
    }
}

static int compare_entries(const void *left, const void *right)
{
    const tinyos_rom_entry *a = left;
    const tinyos_rom_entry *b = right;
    if (a->system != b->system)
        return (int)a->system - (int)b->system;
    return strcasecmp(a->name, b->name);
}

static int scan_directory(tinyos_rom_entry *entries, int capacity, int used,
                          const rom_directory *directory)
{
    DIR *dir = opendir(directory->directory);
    if (!dir)
        return used;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && used < capacity) {
        if (entry->d_name[0] == '.' || !has_extension(entry->d_name, directory->extension))
            continue;

        tinyos_rom_entry *rom = &entries[used];
        if (snprintf(rom->path, sizeof(rom->path), "%s/%s", directory->directory,
                     entry->d_name) >= (int)sizeof(rom->path))
            continue;

        display_name(rom->name, sizeof(rom->name), entry->d_name, directory->extension);
        rom->system = directory->system;
        used++;
    }
    closedir(dir);
    return used;
}

int tinyos_roms_scan(tinyos_rom_entry *entries, int capacity)
{
    if (!entries || capacity <= 0)
        return 0;

    int used = 0;
    for (size_t i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
        used = scan_directory(entries, capacity, used, &directories[i]);
    }

    qsort(entries, (size_t)used, sizeof(entries[0]), compare_entries);
    TINYOS_LOGI("rom library: %d game%s under %s", used, used == 1 ? "" : "s",
                TINYOS_ROMS_ROOT);
    return used;
}

const char *tinyos_rom_system_name(tinyos_rom_system system)
{
    switch (system) {
    case TINYOS_ROM_GB:
        return "Game Boy";
    case TINYOS_ROM_GBC:
        return "Game Boy Color";
    case TINYOS_ROM_GBA:
        return "Game Boy Advance";
    default:
        return "Unknown";
    }
}

const char *tinyos_rom_system_tag(tinyos_rom_system system)
{
    switch (system) {
    case TINYOS_ROM_GB:
        return "GB";
    case TINYOS_ROM_GBC:
        return "GBC";
    case TINYOS_ROM_GBA:
        return "GBA";
    default:
        return "?";
    }
}
