#include "../../platform.h"

#include <stdbool.h>
#include <stdint.h>

static uint32_t pending_key;
static bool key_pending;

static void read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;
    if (key_pending) {
        data->key = pending_key;
        data->state = LV_INDEV_STATE_PRESSED;
        key_pending = false;
    } else {
        data->key = 0;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

lv_indev_t *tinyos_input_create(void)
{
    lv_indev_t *indev = lv_indev_create();
    if (!indev)
        return NULL;
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev, read_cb);
    return indev;
}

void tinyos_host_input_key(uint32_t key)
{
    pending_key = key;
    key_pending = true;
}
