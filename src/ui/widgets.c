#include "widgets.h"

#include "theme.h"

static void animate_opa(void *var, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)value, 0);
}

static void animate_translate_y(void *var, int32_t value)
{
    lv_obj_set_style_translate_y((lv_obj_t *)var, value, 0);
}

static void animate_translate_x(void *var, int32_t value)
{
    lv_obj_set_style_translate_x((lv_obj_t *)var, value, 0);
}

lv_obj_t *tinyos_obj(lv_obj_t *parent, int x, int y, int width, int height)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_scrollable(obj, false);
    return obj;
}

lv_obj_t *tinyos_card(lv_obj_t *parent, int x, int y, int width, int height)
{
    lv_obj_t *obj = tinyos_obj(parent, x, y, width, height);
    lv_obj_add_style(obj, &tinyos_theme.card, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_clip_corner(obj, true, 0);
    return obj;
}

lv_obj_t *tinyos_label(lv_obj_t *parent, const char *text, const lv_font_t *font,
                       uint32_t color, int x, int y, int width, int height)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text ? text : "");
    if (width > 0 && height > 0)
        lv_obj_set_size(label, width, height);
    else if (width > 0)
        lv_obj_set_width(label, width);
    else if (height > 0)
        lv_obj_set_height(label, height);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_scrollable(label, false);
    return label;
}

lv_obj_t *tinyos_icon(lv_obj_t *parent, const char *symbol, uint32_t color,
                      int x, int y, int size)
{
    const lv_font_t *font = &lv_font_montserrat_16;
    if (size >= 26)
        font = &lv_font_montserrat_28;
    else if (size >= 18)
        font = &lv_font_montserrat_20;

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, symbol);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, size, size);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_scrollable(label, false);
    return label;
}

void tinyos_gradient(lv_obj_t *obj, uint32_t top, uint32_t bottom)
{
    lv_obj_set_style_bg_color(obj, lv_color_hex(top), 0);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(bottom), 0);
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_main_stop(obj, 0, 0);
    lv_obj_set_style_bg_grad_stop(obj, 255, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
}

void tinyos_focus(lv_obj_t *obj)
{
    lv_obj_set_clickable(obj, true);
    lv_obj_add_style(obj, &tinyos_theme.focus, LV_STATE_FOCUSED);
}

void tinyos_add_click(lv_obj_t *obj, lv_event_cb_t callback, void *data)
{
    lv_obj_set_clickable(obj, true);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_CLICKED, data);
}

void tinyos_reveal(lv_obj_t *obj, uint32_t delay_ms)
{
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_translate_y(obj, 12, 0);

    lv_anim_t fade;
    lv_anim_init(&fade);
    lv_anim_set_var(&fade, obj);
    lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&fade, 260);
    lv_anim_set_delay(&fade, delay_ms);
    lv_anim_set_exec_cb(&fade, animate_opa);
    lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
    lv_anim_start(&fade);

    lv_anim_t slide;
    lv_anim_init(&slide);
    lv_anim_set_var(&slide, obj);
    lv_anim_set_values(&slide, 12, 0);
    lv_anim_set_duration(&slide, 300);
    lv_anim_set_delay(&slide, delay_ms);
    lv_anim_set_exec_cb(&slide, animate_translate_y);
    lv_anim_set_path_cb(&slide, lv_anim_path_ease_out);
    lv_anim_start(&slide);
}

void tinyos_content_in(lv_obj_t *obj)
{
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_translate_x(obj, 10, 0);

    lv_anim_t fade;
    lv_anim_init(&fade);
    lv_anim_set_var(&fade, obj);
    lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&fade, 200);
    lv_anim_set_exec_cb(&fade, animate_opa);
    lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
    lv_anim_start(&fade);

    lv_anim_t slide;
    lv_anim_init(&slide);
    lv_anim_set_var(&slide, obj);
    lv_anim_set_values(&slide, 10, 0);
    lv_anim_set_duration(&slide, 240);
    lv_anim_set_exec_cb(&slide, animate_translate_x);
    lv_anim_set_path_cb(&slide, lv_anim_path_ease_out);
    lv_anim_start(&slide);
}
