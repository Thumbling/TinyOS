#ifndef TINYOS_UI_SCALE_H
#define TINYOS_UI_SCALE_H

#include <lvgl/lvgl.h>
#include <stdbool.h>

/* User-selectable text scale. Only the type ramp changes: boxes, focus rings
 * and rows are adapted per screen so the 240 px canvas never overflows. */
int tinyos_text_scale(void);
bool tinyos_large_text(void);
int tinyos_text_px(int base);

/* Maps a requested base font (a Montserrat size) onto the active family at
 * the scaled size. Unknown/custom font pointers pass through untouched. */
const lv_font_t *tinyos_font_scale(const lv_font_t *font);

#endif
