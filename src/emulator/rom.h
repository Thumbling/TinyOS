#ifndef TINYOS_EMULATOR_ROM_H
#define TINYOS_EMULATOR_ROM_H

#include <stddef.h>

#define TINYOS_ROM_MAX 128
#define TINYOS_ROM_NAME_MAX 64
#define TINYOS_ROM_PATH_MAX 320

#ifndef TINYOS_ROMS_ROOT
#define TINYOS_ROMS_ROOT "/data/roms"
#endif

typedef enum {
    TINYOS_ROM_GB = 0,
    TINYOS_ROM_GBC,
    TINYOS_ROM_GBA,
} tinyos_rom_system;

typedef struct {
    char name[TINYOS_ROM_NAME_MAX];
    char path[TINYOS_ROM_PATH_MAX];
    tinyos_rom_system system;
} tinyos_rom_entry;

/* Scans /data/roms/{gb,gbc,gba}. Returns the number of entries written.
 * Unreadable or missing directories are simply skipped. */
int tinyos_roms_scan(tinyos_rom_entry *entries, int capacity);

const char *tinyos_rom_system_name(tinyos_rom_system system);
const char *tinyos_rom_system_tag(tinyos_rom_system system);

#endif
