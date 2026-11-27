/* ARM link proof for the production emulator wrapper and real platform glue.
 *
 * It is never part of a firmware image. Linking this file forces emulator.c,
 * input_evdev.c, display_fbdev.c and audio_alsa.c into an ARM executable.
 * tinyos_ctx_get() is the only app-layer function stubbed, because the link
 * audit deliberately does not pull in the whole LVGL UI application. */
#include "app.h"
#include "emulator/emulator.h"

#include <stdlib.h>

tinyos_ctx *tinyos_ctx_get(void)
{
    return NULL;
}

int main(void)
{
    /* Keep this call reachable so the linker cannot drop the production
     * emulator path. The binary is inspected, not executed. */
    int (*run)(const tinyos_rom_entry *) = tinyos_emulator_run;
    if (getenv("TINYOS_ARM_LINK_RUN")) {
        tinyos_rom_entry rom = {0};
        return run(&rom);
    }
    return 0;
}
