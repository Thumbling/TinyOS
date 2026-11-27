#ifndef TINYOS_UI_WIDGETS_H
#define TINYOS_UI_WIDGETS_H

#include <lvgl/lvgl.h>
#include <stdint.h>

lv_obj_t *tinyos_obj(lv_obj_t *parent, int x, int y, int width, int height);
lv_obj_t *tinyos_card(lv_obj_t *parent, int x, int y, int width, int height);
lv_obj_t *tinyos_label(lv_obj_t *parent, const char *text, const lv_font_t *font,
                       uint32_t color, int x, int y, int width, int height);
lv_obj_t *tinyos_icon(lv_obj_t *parent, const char *symbol, uint32_t color,
                      int x, int y, int size);
lv_obj_t *tinyos_back_button(lv_obj_t *parent, lv_event_cb_t callback);
lv_obj_t *tinyos_page_title(lv_obj_t *parent, const char *text, int width);
void tinyos_gradient(lv_obj_t *obj, uint32_t top, uint32_t bottom);
void tinyos_focus(lv_obj_t *obj);
void tinyos_focus_on_accent(lv_obj_t *obj);
void tinyos_add_click(lv_obj_t *obj, lv_event_cb_t callback, void *data);
void tinyos_reveal(lv_obj_t *obj, uint32_t delay_ms);
void tinyos_content_in(lv_obj_t *obj);
void tinyos_pop_in(lv_obj_t *obj);
void tinyos_nudge(lv_obj_t *obj, int dx);

#endif
