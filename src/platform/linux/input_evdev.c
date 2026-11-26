/*
 * TinyOS evdev keypad backend.
 *
 * All RG Nano face buttons and shoulders are exposed as Linux input events by
 * the pcal6416 GPIO expander (see the mainline device tree). We translate the
 * first key of each poll into an LVGL keypad event. Holding a key is left to
 * LVGL's focus animations; the device has no analog controls.
 */
#include "../../platform.h"
#include "../../core/key_repeat.h"
#include "../../core/log.h"

#include <fcntl.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_INPUTS 8
#define MAX_EVENT_NODES 32

typedef struct {
    int fds[MAX_INPUTS];
    int fd_count;
    lv_indev_t *indev;
} evdev_input;

static uint32_t translate_key(uint16_t code)
{
    switch (code) {
    case BTN_DPAD_UP:
        return LV_KEY_UP;
    case BTN_DPAD_DOWN:
        return LV_KEY_DOWN;
    case BTN_DPAD_LEFT:
        return LV_KEY_LEFT;
    case BTN_DPAD_RIGHT:
        return LV_KEY_RIGHT;
    case BTN_EAST: /* A */
        return LV_KEY_ENTER;
    case BTN_SOUTH: /* B */
        return LV_KEY_ESC;
    case BTN_NORTH: /* X */
        return TINYOS_KEY_PREV;
    case BTN_WEST: /* Y */
        return TINYOS_KEY_NEXT;
    case BTN_TL:
        return TINYOS_KEY_PREV;
    case BTN_TR:
        return TINYOS_KEY_NEXT;
    case BTN_START:
        return LV_KEY_HOME;
    case BTN_SELECT:
        return LV_KEY_ESC;
    case KEY_POWER:
        return LV_KEY_ESC;
    default:
        return 0;
    }
}

static void read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    evdev_input *input = lv_indev_get_user_data(indev);
    struct input_event event;
    uint32_t now = lv_tick_get();

    for (int i = 0; i < input->fd_count; i++) {
        while (read(input->fds[i], &event, sizeof(event)) == (ssize_t)sizeof(event)) {
            if (event.type != EV_KEY)
                continue;
            uint32_t key = translate_key(event.code);
            if (key == 0)
                continue;
            /* value 1 = physical press, 0 = physical release, 2 = kernel
             * repeat. The kernel rate is not ours to control; treat the
             * press as a hold and let key_repeat own the cadence. */
            if (event.value == 1)
                tinyos_key_repeat_down(key, now);
            else if (event.value == 0)
                tinyos_key_repeat_up(key, now);
        }
    }

    uint32_t key = tinyos_key_repeat_poll(now);
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
    evdev_input *input = lv_malloc_zeroed(sizeof(*input));
    if (!input)
        return NULL;
    input->fd_count = 0;
    tinyos_key_repeat_reset();

    for (int index = 0; index < MAX_EVENT_NODES && input->fd_count < MAX_INPUTS; index++) {
        char path[64];
        snprintf(path, sizeof(path), "/dev/input/event%d", index);
        int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
        if (fd < 0)
            continue;

        char name[128] = "unknown";
        ioctl(fd, EVIOCGNAME(sizeof(name) - 1), name);
        name[sizeof(name) - 1] = '\0';
        TINYOS_LOGI("input %s: %s", path, name);
        input->fds[input->fd_count++] = fd;
    }

    if (input->fd_count == 0)
        TINYOS_LOGW("no evdev devices found");

    input->indev = lv_indev_create();
    if (!input->indev) {
        for (int i = 0; i < input->fd_count; i++)
            close(input->fds[i]);
        lv_free(input);
        return NULL;
    }

    lv_indev_set_type(input->indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(input->indev, read_cb);
    lv_indev_set_user_data(input->indev, input);
    return input->indev;
}
