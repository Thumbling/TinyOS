#ifndef TINYOS_EMULATOR_EMULATOR_H
#define TINYOS_EMULATOR_EMULATOR_H

#include "rom.h"

/* A game session owns the panel and the input device until it returns.
 * status is 0 on a normal user exit, -1 when the ROM could not be started. */
int tinyos_emulator_run(const tinyos_rom_entry *rom);

const char *tinyos_emulator_last_error(void);

#endif
