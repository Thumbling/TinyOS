#include "module.h"

#include "../platform.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    tinyos_ctx *ctx;
    lv_obj_t *title;
    lv_obj_t *artist;
    lv_obj_t *format;
    lv_obj_t *bar;
    lv_obj_t *position;
    lv_obj_t *duration;
    lv_obj_t *play_icon;
    lv_timer_t *timer;
} music_view;

static void format_time(char *buffer, size_t size, int milliseconds)
{
    if (milliseconds < 0)
        milliseconds = 0;
    int seconds = milliseconds / 1000;
    snprintf(buffer, size, "%d:%02d", seconds / 60, seconds % 60);
}

static void music_refresh(music_view *view)
{
    if (!view || !view->ctx->audio)
        return;

    const tinyos_track *track = tinyos_audio_current_track(view->ctx->audio);
    int playing = tinyos_audio_is_playing(view->ctx->audio);
    lv_label_set_text(view->play_icon, playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);

    if (!track) {
        lv_label_set_text(view->title, "No tracks yet");
        lv_label_set_text(view->artist, "Copy WAV files to /data/music");
        lv_label_set_text(view->format, "WAV playback");
        lv_label_set_text(view->position, "0:00");
        lv_label_set_text(view->duration, "0:00");
        lv_bar_set_value(view->bar, 0, LV_ANIM_OFF);
        return;
    }

    lv_label_set_text(view->title, track->title);
    lv_label_set_text(view->artist, "TinyOS Sessions");
    int duration = tinyos_audio_duration_ms(view->ctx->audio);
    int position = tinyos_audio_position_ms(view->ctx->audio);
    char left[16];
    char right[16];
    format_time(left, sizeof(left), position);
    format_time(right, sizeof(right), duration);
    lv_label_set_text(view->position, left);
    lv_label_set_text(view->duration, right);
    lv_label_set_text(view->format, "WAV - local");

    int value = duration > 0 ? (position * 1000) / duration : 0;
    lv_bar_set_value(view->bar, value,
                     view->ctx->settings.reduce_motion ? LV_ANIM_OFF : LV_ANIM_ON);
}

static void music_timer_cb(lv_timer_t *timer)
{
    music_refresh(lv_timer_get_user_data(timer));
}

static void play_cb(lv_event_t *event)
{
    music_view *view = lv_event_get_user_data(event);
    tinyos_audio_toggle(view->ctx->audio);
    music_refresh(view);
}

static void prev_cb(lv_event_t *event)
{
    music_view *view = lv_event_get_user_data(event);
    tinyos_audio_next(view->ctx->audio, -1);
    music_refresh(view);
}

static void music_key_cb(lv_event_t *event)
{
    music_view *view = lv_event_get_user_data(event);
    uint32_t key = lv_event_get_key(event);
    if (!view)
        return;
    if (key == TINYOS_KEY_PREV) {
        tinyos_audio_next(view->ctx->audio, -1);
        music_refresh(view);
    } else if (key == TINYOS_KEY_NEXT) {
        tinyos_audio_next(view->ctx->audio, 1);
        music_refresh(view);
    }
}

static void next_cb(lv_event_t *event)
{
    music_view *view = lv_event_get_user_data(event);
    tinyos_audio_next(view->ctx->audio, 1);
    music_refresh(view);
}

static void back_cb(lv_event_t *event)
{
    tinyos_shell_back(tinyos_ctx_get());
}

static void rescan_cb(lv_event_t *event)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    tinyos_audio_scan(ctx->audio);
    tinyos_shell_show(ctx, "music");
}

static void music_delete_cb(lv_event_t *event)
{
    music_view *view = lv_event_get_user_data(event);
    if (!view)
        return;
    if (view->timer)
        lv_timer_delete(view->timer);
    free(view);
}

static lv_obj_t *icon_button(lv_obj_t *parent, int x, int y, int size, int glyph,
                             const char *symbol, uint32_t background, uint32_t color,
                             lv_event_cb_t callback, void *data)
{
    lv_obj_t *button = tinyos_obj(parent, x, y, size, size);
    lv_obj_add_style(button, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(background), 0);
    tinyos_focus(button);
    tinyos_add_click(button, callback, data);
    lv_obj_add_event_cb(button, music_key_cb, LV_EVENT_KEY, data);
    lv_obj_t *icon = tinyos_icon(button, symbol, color, 0, 0, glyph);
    lv_obj_center(icon);
    return button;
}

static lv_obj_t *music_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    music_view *view = calloc(1, sizeof(*view));
    if (!view)
        return parent;
    view->ctx = ctx;

    lv_obj_t *back = icon_button(parent, 10, 2, 26, 16, LV_SYMBOL_LEFT,
                                 TINYOS_COLOR_SURFACE_2,
                                 TINYOS_COLOR_MUTED, back_cb, view);
    tinyos_label(parent, "Music", &lv_font_montserrat_20, TINYOS_COLOR_TEXT,
                 44, 2, 140, 24);
    int count = tinyos_audio_count(ctx->audio);
    char subtitle[32];
    snprintf(subtitle, sizeof(subtitle), "%d track%s", count, count == 1 ? "" : "s");
    view->format = tinyos_label(parent, subtitle, &lv_font_montserrat_10,
                                TINYOS_COLOR_DIM, 168, 8, 58, 13);
    lv_obj_set_style_text_align(view->format, LV_TEXT_ALIGN_RIGHT, 0);

    if (count == 0) {
        lv_obj_t *empty = tinyos_card(parent, 14, 44, 212, 96);
        tinyos_gradient(empty, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
        tinyos_icon(empty, LV_SYMBOL_AUDIO, TINYOS_COLOR_CYAN, 86, 12, 40);
        lv_obj_t *empty_title = tinyos_label(empty, "No tracks yet",
                                             &lv_font_montserrat_14, TINYOS_COLOR_TEXT,
                                             0, 54, 212, 20);
        lv_obj_set_style_text_align(empty_title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_t *empty_sub = tinyos_label(empty, "Copy WAV files to /data/music",
                                           &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                           0, 76, 212, 13);
        lv_obj_set_style_text_align(empty_sub, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *rescan = tinyos_obj(parent, 78, 146, 84, 20);
        lv_obj_add_style(rescan, &tinyos_theme.pill, 0);
        lv_obj_set_style_bg_color(rescan, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
        tinyos_focus(rescan);
        tinyos_add_click(rescan, rescan_cb, NULL);
        lv_obj_add_event_cb(rescan, music_key_cb, LV_EVENT_KEY, view);
        lv_obj_t *label = tinyos_label(rescan, "RESCAN", &lv_font_montserrat_10,
                                       TINYOS_COLOR_CYAN, 0, 6, 84, 10);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        tinyos_shell_add_focus(ctx, rescan);
        tinyos_shell_add_focus(ctx, back);

        lv_obj_add_event_cb(parent, music_delete_cb, LV_EVENT_DELETE, view);
        return parent;
    }

    lv_obj_t *art = tinyos_card(parent, 14, 34, 60, 60);
    tinyos_gradient(art, TINYOS_COLOR_CYAN_DEEP, TINYOS_COLOR_VIOLET);
    lv_obj_set_style_radius(art, 14, 0);
    lv_obj_t *disc = tinyos_obj(art, 10, 10, 40, 40);
    lv_obj_set_style_radius(disc, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(disc, lv_color_hex(TINYOS_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(disc, LV_OPA_80, 0);
    tinyos_icon(disc, LV_SYMBOL_AUDIO, TINYOS_COLOR_TEXT, 10, 10, 20);

    view->title = tinyos_label(parent, "-", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                               86, 38, 140, 22);
    view->artist = tinyos_label(parent, "TinyOS Sessions", &lv_font_montserrat_10,
                                TINYOS_COLOR_MUTED, 86, 63, 140, 13);
    view->format = tinyos_label(parent, "WAV - local", &lv_font_montserrat_10,
                                TINYOS_COLOR_DIM, 86, 78, 140, 13);

    view->bar = lv_bar_create(parent);
    lv_obj_set_pos(view->bar, 14, 102);
    lv_obj_set_size(view->bar, 212, 4);
    lv_bar_set_range(view->bar, 0, 1000);
    lv_obj_set_style_bg_color(view->bar, lv_color_hex(TINYOS_COLOR_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(view->bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(view->bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(view->bar, lv_color_hex(TINYOS_COLOR_CYAN), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(view->bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(view->bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_anim_duration(view->bar, 450, 0);

    view->position = tinyos_label(parent, "0:00", &lv_font_montserrat_10,
                                  TINYOS_COLOR_MUTED, 14, 108, 60, 12);
    view->duration = tinyos_label(parent, "0:00", &lv_font_montserrat_10,
                                  TINYOS_COLOR_MUTED, 166, 108, 60, 12);
    lv_obj_set_style_text_align(view->duration, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *prev = icon_button(parent, 66, 132, 28, 16, LV_SYMBOL_PREV,
                                 TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_MUTED, prev_cb, view);
    lv_obj_t *play = icon_button(parent, 98, 124, 44, 22, LV_SYMBOL_PLAY,
                                 TINYOS_COLOR_CYAN, TINYOS_COLOR_ON_ACCENT, play_cb, view);
    lv_obj_t *next = icon_button(parent, 146, 132, 28, 16, LV_SYMBOL_NEXT,
                                 TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_MUTED, next_cb, view);

    view->play_icon = lv_obj_get_child(play, 0);
    /* No large glow: it washed out the hint line and costs a shadow layer. */

    tinyos_shell_add_focus(ctx, play);
    tinyos_shell_add_focus(ctx, prev);
    tinyos_shell_add_focus(ctx, next);
    tinyos_shell_add_focus(ctx, back);

    view->timer = lv_timer_create(music_timer_cb, 700, view);
    lv_obj_add_event_cb(parent, music_delete_cb, LV_EVENT_DELETE, view);

    tinyos_reveal(art, 0);
    tinyos_reveal(view->title, 40);
    tinyos_reveal(play, 80);
    music_refresh(view);
    return parent;
}

const tinyos_module tinyos_module_music = {
    .id = "music",
    .name = "Music",
    .subtitle = "WAV player",
    .icon = LV_SYMBOL_AUDIO,
    .create = music_create,
};
