#ifndef TINYOS_CORE_KEY_REPEAT_H
#define TINYOS_CORE_KEY_REPEAT_H

#include <stdint.h>

/* Platform-independent key repeat policy.
 *
 * Directions repeat only after the key has been held, actions (A, B, Start,
 * Select, prev/next) never repeat. Repeats are produced on demand from poll(),
 * so a slow frame can never build a backlog, and releasing the key cancels the
 * next repeat immediately. Both the evdev and host backends feed this module;
 * the simulator's --keytest drives it with a virtual clock. */
#define TINYOS_KEY_REPEAT_DELAY_MS 350u
#define TINYOS_KEY_REPEAT_INTERVAL_MS 90u

void tinyos_key_repeat_reset(void);
void tinyos_key_repeat_down(uint32_t key, uint32_t now_ms);
void tinyos_key_repeat_up(uint32_t key, uint32_t now_ms);

/* Returns one key event for this frame, or 0. Never returns two at once. */
uint32_t tinyos_key_repeat_poll(uint32_t now_ms);

#endif
