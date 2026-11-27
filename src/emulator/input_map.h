#ifndef TINYOS_EMULATOR_INPUT_MAP_H
#define TINYOS_EMULATOR_INPUT_MAP_H

#include <stdint.h>

/* Linux input-event ABI values, kept here so the complete physical-button
 * mapping can be exercised by the host self-test without a device node. */
#define TINYOS_EV_BTN_SOUTH 0x130
#define TINYOS_EV_BTN_EAST 0x131
#define TINYOS_EV_BTN_NORTH 0x133
#define TINYOS_EV_BTN_WEST 0x134
#define TINYOS_EV_BTN_TL 0x136
#define TINYOS_EV_BTN_TR 0x137
#define TINYOS_EV_BTN_SELECT 0x13a
#define TINYOS_EV_BTN_START 0x13b
#define TINYOS_EV_BTN_DPAD_UP 0x220
#define TINYOS_EV_BTN_DPAD_DOWN 0x221
#define TINYOS_EV_BTN_DPAD_LEFT 0x222
#define TINYOS_EV_BTN_DPAD_RIGHT 0x223
#define TINYOS_EV_KEY_POWER 116

/* Maps a Linux evdev key to the direct-game input bits. Returns 0 for keys
 * TinyOS does not expose to a game session. */
uint32_t tinyos_game_key_from_evdev(uint16_t code);

#endif
