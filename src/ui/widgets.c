#include "widgets.h"

#include "scale.h"

#include "fonts.h"
#include "shell.h"
#include "theme.h"

#include <limits.h>
#include <stdlib.h>

static bool reduce_motion(void)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    return ctx && ctx->settings.reduce_motion;
}

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
    const lv_font_t *scaled = tinyos_font_scale(font);
    lv_obj_set_style_text_font(label, scaled, 0);
    if (tinyos_large_text() && height > 0) {
        int line = lv_font_get_line_height(scaled) + 1;
        if (height < line) {
            height = line;
            if (width > 0)
                lv_obj_set_size(label, width, height);
            else
                lv_obj_set_height(label, height);
        }
    }
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_scrollable(label, false);
    return label;
}

lv_obj_t *tinyos_icon(lv_obj_t *parent, const char *symbol, uint32_t color,
                      int x, int y, int size)
{
    /* The glyph has to fit the box it is given: a 16 px font in a 10 px box
     * is exactly the clipped arrow the design review found. Pick the font
     * that survives the box and centre it inside, so a mark never loses its
     * tips no matter which call site places it. */
    const lv_font_t *font = &lv_font_montserrat_10;
    if (size >= 26)
        font = &lv_font_montserrat_28;
    else if (size >= 18)
        font = &lv_font_montserrat_20;
    else if (size >= 14)
        font = &lv_font_montserrat_16;
    else if (size >= 12)
        font = &lv_font_montserrat_12;

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, symbol);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, size, size);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_set_style_pad_top(label, (size - lv_font_get_line_height(font)) / 2, 0);
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

static int center_x(lv_obj_t *obj)
{
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    return (area.x1 + area.x2) / 2;
}

static int center_y(lv_obj_t *obj)
{
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    return (area.y1 + area.y2) / 2;
}

static lv_obj_t *scroll_parent(lv_obj_t *obj)
{
    lv_obj_t *parent = lv_obj_get_parent(obj);
    while (parent && parent != lv_screen_active()
           && lv_obj_get_scroll_dir(parent) == LV_DIR_NONE)
        parent = lv_obj_get_parent(parent);
    return (parent && parent != lv_screen_active()) ? parent : NULL;
}

static lv_obj_t *spatial_focus(lv_group_t *group, uint32_t key)
{
    lv_obj_t *current = lv_group_get_focused(group);
    if (!current)
        return NULL;

    int current_x = center_x(current);
    int current_y = center_y(current);
    lv_obj_t *current_scroll = scroll_parent(current);
    int best_score = INT_MAX;
    lv_obj_t *best = NULL;
    uint32_t count = lv_group_get_obj_count(group);
    for (uint32_t i = 0; i < count; i++) {
        lv_obj_t *candidate = lv_group_get_obj_by_index(group, i);
        if (!candidate || candidate == current)
            continue;
        /* Rows below a list viewport are still legitimate focus targets; the
         * focus_into_view() call scrolls them in. Without this, the first key
         * down from the last visible row jumped into the tab bar instead of
         * continuing through the list. */
        lv_obj_t *candidate_scroll = scroll_parent(candidate);
        if (!lv_obj_is_visible(candidate)) {
            if (!current_scroll || current_scroll != candidate_scroll)
                continue;
        }
        if (lv_obj_has_state(candidate, LV_STATE_DISABLED))
            continue;

        int x = center_x(candidate);
        int y = center_y(candidate);
        int dx = x - current_x;
        int dy = y - current_y;
        bool horizontal = key == LV_KEY_LEFT || key == LV_KEY_RIGHT;

        if (horizontal) {
            if ((key == LV_KEY_RIGHT && dx <= 4) || (key == LV_KEY_LEFT && dx >= -4))
                continue;
        } else {
            if ((key == LV_KEY_DOWN && dy <= 4) || (key == LV_KEY_UP && dy >= -4))
                continue;
        }

        int primary = horizontal ? abs(dx) : abs(dy);
        int secondary = horizontal ? abs(dy) : abs(dx);
        int score = primary * 4 + secondary;
        /* While a list has focus, exhaust it before entering the tab bar,
         * even when the next row is still clipped below the viewport. */
        if (current_scroll && candidate_scroll != current_scroll)
            score += 10000;
        if (score < best_score) {
            best_score = score;
            best = candidate;
        }
    }
    return best;
}

static void focus_into_view(lv_obj_t *obj)
{
    lv_obj_t *parent = lv_obj_get_parent(obj);
    while (parent && lv_obj_get_scroll_dir(parent) == LV_DIR_NONE)
        parent = lv_obj_get_parent(parent);
    if (parent && parent != lv_screen_active())
        lv_obj_scroll_to_view(obj, reduce_motion() ? LV_ANIM_OFF : LV_ANIM_ON);
}

static void global_key_cb(lv_event_t *event)
{
    uint32_t key = lv_event_get_key(event);
    tinyos_ctx *ctx = tinyos_ctx_get();
    lv_group_t *group = ctx->input ? lv_indev_get_group(ctx->input) : ctx->group;
    if (!group)
        return;

    if (key == LV_KEY_ESC) {
        tinyos_shell_back(ctx);
        lv_event_stop_processing(event);
    } else if (key == LV_KEY_HOME) {
        tinyos_shell_show(ctx, "home");
        lv_event_stop_processing(event);
    } else if (key == LV_KEY_UP || key == LV_KEY_DOWN ||
               key == LV_KEY_LEFT || key == LV_KEY_RIGHT) {
        if (lv_group_get_editing(group))
            return;
        lv_obj_t *next = spatial_focus(group, key);
        if (next) {
            lv_group_focus_obj(next);
            focus_into_view(next);
            lv_event_stop_processing(event);
        }
    }
}

void tinyos_focus(lv_obj_t *obj)
{
    lv_obj_set_clickable(obj, true);
    lv_obj_add_style(obj, &tinyos_theme.focus, LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &tinyos_theme.pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(obj, global_key_cb, LV_EVENT_KEY, NULL);
}

void tinyos_add_click(lv_obj_t *obj, lv_event_cb_t callback, void *data)
{
    lv_obj_set_clickable(obj, true);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_CLICKED, data);
}

void tinyos_reveal(lv_obj_t *obj, uint32_t delay_ms)
{
    if (reduce_motion()) {
        lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_translate_y(obj, 0, 0);
        return;
    }
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
    if (reduce_motion()) {
        lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_translate_x(obj, 0, 0);
        return;
    }
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

/* A dialog enters faster than a screen: 150 ms fade + 8 px rise. */
void tinyos_pop_in(lv_obj_t *obj)
{
    if (!obj)
        return;
    if (reduce_motion()) {
        lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_translate_y(obj, 0, 0);
        return;
    }
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_translate_y(obj, 8, 0);

    lv_anim_t fade;
    lv_anim_init(&fade);
    lv_anim_set_var(&fade, obj);
    lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&fade, 150);
    lv_anim_set_exec_cb(&fade, animate_opa);
    lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
    lv_anim_start(&fade);

    lv_anim_t slide;
    lv_anim_init(&slide);
    lv_anim_set_var(&slide, obj);
    lv_anim_set_values(&slide, 8, 0);
    lv_anim_set_duration(&slide, 160);
    lv_anim_set_exec_cb(&slide, animate_translate_y);
    lv_anim_set_path_cb(&slide, lv_anim_path_ease_out);
    lv_anim_start(&slide);
}

/* Value stepping feedback: a 6 px settle, no overshoot. */
void tinyos_nudge(lv_obj_t *obj, int dx)
{
    if (!obj || reduce_motion())
        return;
    lv_anim_delete(obj, animate_translate_x);
    lv_obj_set_style_translate_x(obj, dx, 0);
    lv_anim_t slide;
    lv_anim_init(&slide);
    lv_anim_set_var(&slide, obj);
    lv_anim_set_values(&slide, dx, 0);
    lv_anim_set_duration(&slide, 140);
    lv_anim_set_exec_cb(&slide, animate_translate_x);
    lv_anim_set_path_cb(&slide, lv_anim_path_ease_out);
    lv_anim_start(&slide);
}
