#include "key_repeat.h"

#include <lvgl/lvgl.h>

#include <stdbool.h>
#include <stddef.h>

#define HELD_MAX 6

typedef struct {
    uint32_t key;
    uint32_t next_repeat_ms;
    bool down;
    bool repeatable;
} held_key;

static held_key held[HELD_MAX];
static uint32_t pending_key;

static bool is_direction(uint32_t key)
{
    return key == LV_KEY_UP || key == LV_KEY_DOWN || key == LV_KEY_LEFT
           || key == LV_KEY_RIGHT;
}

static held_key *find(uint32_t key)
{
    for (int i = 0; i < HELD_MAX; i++) {
        if (held[i].down && held[i].key == key)
            return &held[i];
    }
    return NULL;
}

static held_key *slot(void)
{
    for (int i = 0; i < HELD_MAX; i++) {
        if (!held[i].down)
            return &held[i];
    }
    return NULL;
}

static void stop_direction_repeats(void)
{
    for (int i = 0; i < HELD_MAX; i++) {
        if (held[i].down && is_direction(held[i].key))
            held[i].repeatable = false;
    }
}

void tinyos_key_repeat_reset(void)
{
    for (int i = 0; i < HELD_MAX; i++) {
        held[i].down = false;
        held[i].key = 0;
        held[i].next_repeat_ms = 0;
        held[i].repeatable = false;
    }
    pending_key = 0;
}

void tinyos_key_repeat_down(uint32_t key, uint32_t now_ms)
{
    if (key == 0)
        return;

    held_key *existing = find(key);
    if (existing) {
        /* The kernel may repeat a held key; a physical press must not fire
         * twice. Ignore the duplicate until the key is released. */
        return;
    }

    if (!is_direction(key)) {
        /* An action press ends any running navigation repeat. */
        stop_direction_repeats();
    } else {
        /* Only the newest direction repeats; the previous one is stale. */
        for (int i = 0; i < HELD_MAX; i++) {
            if (held[i].down && held[i].key != key && is_direction(held[i].key))
                held[i].repeatable = false;
        }
    }

    held_key *entry = slot();
    if (!entry)
        return;
    entry->key = key;
    entry->down = true;
    entry->repeatable = is_direction(key);
    entry->next_repeat_ms = now_ms + TINYOS_KEY_REPEAT_DELAY_MS;
    pending_key = key;
}

void tinyos_key_repeat_up(uint32_t key, uint32_t now_ms)
{
    (void)now_ms;
    if (key == 0)
        return;
    held_key *entry = find(key);
    if (!entry)
        return;
    entry->down = false;
    entry->repeatable = false;
    entry->next_repeat_ms = 0;
}

uint32_t tinyos_key_repeat_poll(uint32_t now_ms)
{
    if (pending_key != 0) {
        uint32_t key = pending_key;
        pending_key = 0;
        return key;
    }

    for (int i = 0; i < HELD_MAX; i++) {
        if (!held[i].down || !held[i].repeatable)
            continue;
        if ((int32_t)(now_ms - held[i].next_repeat_ms) < 0)
            continue;
        uint32_t key = held[i].key;
        /* Advance from now, not from the missed deadline: a slow frame emits
         * one repeat, never a burst that queues up focus moves. */
        held[i].next_repeat_ms = now_ms + TINYOS_KEY_REPEAT_INTERVAL_MS;
        return key;
    }
    return 0;
}
