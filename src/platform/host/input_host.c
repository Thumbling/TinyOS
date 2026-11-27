#include "../../platform.h"
#include "../../core/key_repeat.h"

#include <stdbool.h>
#include <stdint.h>

static void read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;
    uint32_t key = tinyos_key_repeat_poll(lv_tick_get());
    if (key != 0) {
        data->key = key;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->key = 0;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

lv_indev_t *tinyos_input_create(void)
{
    tinyos_key_repeat_reset();
    lv_indev_t *indev = lv_indev_create();
    if (!indev)
        return NULL;
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev, read_cb);
    return indev;
}

void tinyos_host_input_key(uint32_t key)
{
    tinyos_host_input_down(key);
    tinyos_host_input_up(key);
}

void tinyos_host_input_down(uint32_t key)
{
    tinyos_key_repeat_down(key, lv_tick_get());
}

void tinyos_host_input_up(uint32_t key)
{
    tinyos_key_repeat_up(key, lv_tick_get());
}

static uint32_t host_game_keys;

void tinyos_game_input_begin(void)
{
    host_game_keys = 0;
}

uint32_t tinyos_game_input_poll(void)
{
    return host_game_keys;
}

void tinyos_game_input_end(void)
{
    host_game_keys = 0;
}

void tinyos_host_game_input_set(uint32_t keys)
{
    host_game_keys = keys;
}
