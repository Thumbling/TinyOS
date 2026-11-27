/* ARM link probe for the headless emulator path. It is not run on the host;
 * its purpose is to force the linker to resolve the same session, ROM, input,
 * panel and mGBA symbols used by the firmware game loop. */
#include "emulator/game_keys.h"
#include "emulator/input_map.h"
#include "emulator/rom.h"
#include "emulator/session.h"
#include "platform/linux/fbdev_blit.h"

#include <stdint.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2)
        return 2;

    tinyos_rom_entry rom;
    memset(&rom, 0, sizeof(rom));
    strncpy(rom.path, argv[1], sizeof(rom.path) - 1);
    strncpy(rom.name, "arm link probe", sizeof(rom.name) - 1);
    rom.system = TINYOS_ROM_GBA;

    uint16_t panel[TINYOS_GAME_PANEL_SIZE * TINYOS_GAME_PANEL_SIZE];
    memset(panel, 0, sizeof(panel));

    tinyos_fbdev_geometry geometry;
    memset(&geometry, 0, sizeof(geometry));
    geometry.memory = (uint8_t *)panel;
    geometry.size = sizeof(panel);
    geometry.stride = TINYOS_GAME_PANEL_SIZE * 2;
    geometry.width = TINYOS_GAME_PANEL_SIZE;
    geometry.height = TINYOS_GAME_PANEL_SIZE;
    geometry.bits_per_pixel = 16;
    tinyos_fbdev_blit_rgb565(&geometry, 0, 0, TINYOS_GAME_PANEL_SIZE - 1,
                             TINYOS_GAME_PANEL_SIZE - 1, (uint8_t *)panel);

    tinyos_game_exit_gesture gesture;
    tinyos_game_exit_gesture_init(&gesture);
    tinyos_game_exit_gesture_update(&gesture, TINYOS_GAME_KEY_X |
                                    TINYOS_GAME_KEY_Y, 900);
    unsigned pacing_phase = 0;
    tinyos_game_pacing_delay_ms(&pacing_phase);

    tinyos_emulator_session *session = tinyos_emulator_session_open(&rom);
    if (session) {
        tinyos_emulator_session_run_frame(session, 0);
        (void)tinyos_emulator_session_panel(session);
        (void)tinyos_emulator_session_geometry(session);
        (void)tinyos_emulator_session_clone_save(session, NULL);
        tinyos_emulator_session_close(session, false);
    }

    (void)tinyos_game_key_from_evdev(TINYOS_EV_BTN_EAST);
    (void)tinyos_emulator_controller_keys(TINYOS_GAME_KEY_A, TINYOS_ROM_GBA);
    (void)tinyos_rom_system_name(TINYOS_ROM_GB);
    return 0;
}
