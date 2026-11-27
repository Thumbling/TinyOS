#ifndef TINYOS_EMULATOR_SESSION_H
#define TINYOS_EMULATOR_SESSION_H

#include "rom.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TINYOS_GAME_PANEL_SIZE 240
#define TINYOS_GAME_BARS_GBA 40
#define TINYOS_GAME_BARS_GB 12
#define TINYOS_GAME_EXIT_CHORD_MS 900
#define TINYOS_GAME_SAMPLE_RATE 44100

/* The GBA and Game Boy families both run at 59.7275 Hz. The integer ratio is
 * used for the millisecond pacing pattern and for the audio drift check. */
#define TINYOS_GAME_FPS_NUM 597275u
#define TINYOS_GAME_FPS_DEN 10000u
#define TINYOS_GAME_PACING_WINDOW 10000u
#define TINYOS_GAME_PACING_16MS 2573u

typedef struct tinyos_emulator_session tinyos_emulator_session;

typedef struct {
    unsigned core_width;
    unsigned core_height;
    unsigned panel_width;
    unsigned panel_height;
    unsigned image_x;
    unsigned image_y;
    unsigned image_width;
    unsigned image_height;
} tinyos_emulator_geometry;

typedef struct {
    bool armed;
    bool fired;
    uint32_t started_ms;
} tinyos_game_exit_gesture;

/* A session owns the mGBA core, the converted 240x240 panel and the optional
 * save file. It does not touch LVGL, evdev, ALSA or the display. The firmware
 * loop and the host self-test drive the same session code. */
tinyos_emulator_session *tinyos_emulator_session_open(const tinyos_rom_entry *rom);
void tinyos_emulator_session_close(tinyos_emulator_session *session, bool write_save);
bool tinyos_emulator_session_run_frame(tinyos_emulator_session *session, uint32_t keys);
const uint16_t *tinyos_emulator_session_panel(const tinyos_emulator_session *session);
const tinyos_emulator_geometry *tinyos_emulator_session_geometry(
    const tinyos_emulator_session *session);
const char *tinyos_emulator_session_save_path(const tinyos_emulator_session *session);
size_t tinyos_emulator_session_clone_save(tinyos_emulator_session *session, void **data);
const char *tinyos_emulator_session_last_error(void);
struct mCore *tinyos_emulator_session_core(tinyos_emulator_session *session);

/* Converts the input bits owned by the session to the mGBA core's key bits.
 * X and Y are deliberately not core buttons: they are the local exit chord. */
uint32_t tinyos_emulator_controller_keys(uint32_t keys, tinyos_rom_system system);

void tinyos_game_exit_gesture_init(tinyos_game_exit_gesture *gesture);
bool tinyos_game_exit_gesture_update(tinyos_game_exit_gesture *gesture,
                                     uint32_t keys, uint32_t now_ms);

/* Returns the next 16/17 ms pacing step. Over a 10,000-frame window this is
 * exactly 16.7427 ms per frame, i.e. 59.7275 Hz. */
uint32_t tinyos_game_pacing_delay_ms(unsigned *phase);
uint64_t tinyos_game_audio_samples_for_frames(uint32_t frames);

#endif
