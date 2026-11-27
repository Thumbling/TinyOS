#include "input_map.h"

#include "game_keys.h"

uint32_t tinyos_game_key_from_evdev(uint16_t code)
{
    switch (code) {
    case TINYOS_EV_BTN_DPAD_UP:
        return TINYOS_GAME_KEY_UP;
    case TINYOS_EV_BTN_DPAD_DOWN:
        return TINYOS_GAME_KEY_DOWN;
    case TINYOS_EV_BTN_DPAD_LEFT:
        return TINYOS_GAME_KEY_LEFT;
    case TINYOS_EV_BTN_DPAD_RIGHT:
        return TINYOS_GAME_KEY_RIGHT;
    case TINYOS_EV_BTN_EAST:
        return TINYOS_GAME_KEY_A;
    case TINYOS_EV_BTN_SOUTH:
        return TINYOS_GAME_KEY_B;
    case TINYOS_EV_BTN_NORTH:
        return TINYOS_GAME_KEY_X;
    case TINYOS_EV_BTN_WEST:
        return TINYOS_GAME_KEY_Y;
    case TINYOS_EV_BTN_TL:
        return TINYOS_GAME_KEY_L;
    case TINYOS_EV_BTN_TR:
        return TINYOS_GAME_KEY_R;
    case TINYOS_EV_BTN_START:
        return TINYOS_GAME_KEY_START;
    case TINYOS_EV_BTN_SELECT:
        return TINYOS_GAME_KEY_SELECT;
    case TINYOS_EV_KEY_POWER:
        return TINYOS_GAME_KEY_EXIT;
    default:
        return 0;
    }
}
