#ifndef TINYOS_EMULATOR_GAME_KEYS_H
#define TINYOS_EMULATOR_GAME_KEYS_H

/* Direct game-session input bits. Kept outside platform.h so the host
 * self-test can include the emulator path without pulling in LVGL. */
#define TINYOS_GAME_KEY_A      (1u << 0)
#define TINYOS_GAME_KEY_B      (1u << 1)
#define TINYOS_GAME_KEY_SELECT (1u << 2)
#define TINYOS_GAME_KEY_START  (1u << 3)
#define TINYOS_GAME_KEY_RIGHT  (1u << 4)
#define TINYOS_GAME_KEY_LEFT   (1u << 5)
#define TINYOS_GAME_KEY_UP     (1u << 6)
#define TINYOS_GAME_KEY_DOWN   (1u << 7)
#define TINYOS_GAME_KEY_R      (1u << 8)
#define TINYOS_GAME_KEY_L      (1u << 9)
#define TINYOS_GAME_KEY_X      (1u << 10)
#define TINYOS_GAME_KEY_Y      (1u << 11)
#define TINYOS_GAME_KEY_EXIT   (1u << 12)

#endif
