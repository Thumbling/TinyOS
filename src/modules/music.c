#include "module.h"

#include "music.h"

#include "../platform.h"
#include "../ui/layout.h"
#include "../ui/scale.h"
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

static int forced_state = TINYOS_MUSIC_AUTO;
static bool rescan_pending;
static lv_timer_t *scan_timer;

void tinyos_music_force_state(int state)
{
    forced_state = state;
}

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

static void scan_finished_cb(lv_timer_t *timer)
{
    tinyos_ctx *ctx = lv_timer_get_user_data(timer);
    tinyos_audio_scan(ctx->audio);
    rescan_pending = false;
    scan_timer = NULL;
    tinyos_shell_show(ctx, "music");
}

static void rescan_cb(lv_event_t *event)
{
    (void)event;
    tinyos_ctx *ctx = tinyos_ctx_get();
    /* A failed audio init is recoverable: RETRY creates the backend again. */
    if (!ctx->audio)
        ctx->audio = tinyos_audio_create();
    /* Show the loading state before the (synchronous) scan. The one-shot
     * timer gives the frame a chance to present it, then rebuilds the view. */
    if (ctx->audio && !scan_timer) {
        rescan_pending = true;
        tinyos_shell_show(ctx, "music");
        scan_timer = lv_timer_create(scan_finished_cb, 320, ctx);
        lv_timer_set_repeat_count(scan_timer, 1);
    }
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

static lv_obj_t *state_button(lv_obj_t *parent, int x, int y, int width,
                              const char *text, lv_event_cb_t callback, void *data)
{
    bool large = tinyos_large_text();
    int height = large ? 24 : 22;
    lv_obj_t *button = tinyos_obj(parent, x, y, width, height);
    lv_obj_add_style(button, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    tinyos_focus(button);
    tinyos_add_click(button, callback, data);
    lv_obj_t *label = tinyos_label(button, text, &lv_font_montserrat_10,
                                   TINYOS_COLOR_CYAN, 0, large ? 4 : 6, width,
                                   large ? 15 : 11);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return button;
}

static lv_obj_t *state_card(tinyos_ctx *ctx, lv_obj_t *parent, const char *title,
                            const char *subtitle, const char *symbol,
                            uint32_t accent)
{
    (void)ctx;
    bool large = tinyos_large_text();
    lv_obj_t *card = tinyos_card(parent, TINYOS_CONTENT_X, large ? 40 : 44,
                                 TINYOS_CONTENT_W, 96);
    tinyos_gradient(card, TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_SURFACE);
    tinyos_icon(card, symbol, accent, 88, 12, 36);

    lv_obj_t *heading = tinyos_label(card, title, &lv_font_montserrat_14,
                                     TINYOS_COLOR_TEXT, 6, 52, 200, 22);
    lv_obj_set_style_text_align(heading, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *copy = tinyos_label(card, subtitle, &lv_font_montserrat_10,
                                  TINYOS_COLOR_MUTED, 6, 78, 200, 16);
    lv_obj_set_style_text_align(copy, LV_TEXT_ALIGN_CENTER, 0);
    return card;
}

static lv_obj_t *build_view(tinyos_ctx *ctx, lv_obj_t *parent, music_view *view);

static lv_obj_t *music_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    music_view *view = calloc(1, sizeof(*view));
    if (!view)
        return parent;
    view->ctx = ctx;
    bool large = tinyos_large_text();

    lv_obj_t *back = tinyos_back_button(parent, back_cb);
    tinyos_page_title(parent, "Music", 112);
    int count = ctx->audio ? tinyos_audio_count(ctx->audio) : 0;
    char counter[32];
    snprintf(counter, sizeof(counter), "%d track%s", count, count == 1 ? "" : "s");
    lv_obj_t *count_label = tinyos_label(parent, counter, &lv_font_montserrat_10,
                                         TINYOS_COLOR_DIM, 168, 8, 58, 13);
    lv_obj_set_style_text_align(count_label, LV_TEXT_ALIGN_RIGHT, 0);

    int state = forced_state;
    if (state == TINYOS_MUSIC_AUTO) {
        if (!ctx->audio)
            state = TINYOS_MUSIC_ERROR;
        else if (rescan_pending)
            state = TINYOS_MUSIC_LOADING;
        else
            state = count == 0 ? TINYOS_MUSIC_EMPTY : TINYOS_MUSIC_NORMAL;
    }

    /* A count is only meaningful for the normal player; in a loading, empty
     * or error view it would contradict the central message. */
    if (state != TINYOS_MUSIC_NORMAL)
        lv_label_set_text(count_label, "");

    if (state == TINYOS_MUSIC_LOADING) {
        state_card(ctx, parent, "Scanning music", "Reading /data/music",
                   LV_SYMBOL_REFRESH, TINYOS_COLOR_CYAN);
        int width = large ? 100 : 84;
        lv_obj_t *back_btn = state_button(parent, (212 - width) / 2 + 14,
                                          large ? 142 : 144, width, "BACK",
                                          back_cb, view);
        tinyos_shell_add_focus(ctx, back_btn);
        tinyos_shell_add_focus(ctx, back);
        lv_obj_add_event_cb(parent, music_delete_cb, LV_EVENT_DELETE, view);
        return parent;
    }

    if (state == TINYOS_MUSIC_ERROR) {
        state_card(ctx, parent, "Can't read music",
                   large ? "Check the SD card." : "Check the SD card, then retry.",
                   LV_SYMBOL_WARNING, TINYOS_COLOR_RED);
        int retry_w = large ? 84 : 72;
        int back_w = large ? 64 : 56;
        int gap = 8;
        int x = (212 - (retry_w + gap + back_w)) / 2 + 14;
        lv_obj_t *retry = state_button(parent, x, large ? 142 : 144, retry_w,
                                       "RETRY", rescan_cb, view);
        lv_obj_t *back_btn = state_button(parent, x + retry_w + gap,
                                          large ? 142 : 144, back_w, "BACK",
                                          back_cb, view);
        tinyos_shell_add_focus(ctx, retry);
        tinyos_shell_add_focus(ctx, back_btn);
        tinyos_shell_add_focus(ctx, back);
        lv_obj_add_event_cb(parent, music_delete_cb, LV_EVENT_DELETE, view);
        return parent;
    }

    if (state == TINYOS_MUSIC_EMPTY) {
        state_card(ctx, parent, "No tracks yet",
                   large ? "WAV files: /data/music" : "Copy WAV files to /data/music",
                   LV_SYMBOL_AUDIO, TINYOS_COLOR_CYAN);
        int width = large ? 100 : 84;
        lv_obj_t *rescan = state_button(parent, (212 - width) / 2 + 14,
                                        large ? 142 : 144, width, "RESCAN",
                                        rescan_cb, view);
        tinyos_shell_add_focus(ctx, rescan);
        tinyos_shell_add_focus(ctx, back);
        lv_obj_add_event_cb(parent, music_delete_cb, LV_EVENT_DELETE, view);
        return parent;
    }

    build_view(ctx, parent, view);

    tinyos_shell_add_focus(ctx, back);
    view->timer = lv_timer_create(music_timer_cb, 700, view);
    lv_obj_add_event_cb(parent, music_delete_cb, LV_EVENT_DELETE, view);
    music_refresh(view);
    return parent;
}

static lv_obj_t *build_view(tinyos_ctx *ctx, lv_obj_t *parent, music_view *view)
{
    bool large = tinyos_large_text();
    int art_size = large ? 52 : 60;
    int art_x = TINYOS_CONTENT_X;
    int art_y = large ? 38 : 34;
    lv_obj_t *art = tinyos_card(parent, art_x, art_y, art_size, art_size);
    tinyos_gradient(art, TINYOS_COLOR_CYAN_DEEP, TINYOS_COLOR_VIOLET);
    lv_obj_set_style_radius(art, 14, 0);
    int inset = large ? 9 : 10;
    int disc_size = art_size - inset * 2;
    lv_obj_t *disc = tinyos_obj(art, inset, inset, disc_size, disc_size);
    lv_obj_set_style_radius(disc, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(disc, lv_color_hex(TINYOS_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(disc, LV_OPA_80, 0);
    int glyph = large ? 18 : 20;
    tinyos_icon(disc, LV_SYMBOL_AUDIO, TINYOS_COLOR_TEXT,
                (disc_size - glyph) / 2, (disc_size - glyph) / 2, glyph);

    view->title = tinyos_label(parent, "-", &lv_font_montserrat_16, TINYOS_COLOR_TEXT,
                               large ? 72 : 86, large ? 36 : 38, large ? 154 : 140,
                               large ? 26 : 22);
    view->artist = tinyos_label(parent, "TinyOS Sessions", &lv_font_montserrat_10,
                                TINYOS_COLOR_MUTED, large ? 72 : 86,
                                large ? 62 : 63, large ? 154 : 140, large ? 18 : 13);
    view->format = tinyos_label(parent, "WAV - local", &lv_font_montserrat_10,
                                TINYOS_COLOR_DIM, large ? 72 : 86,
                                large ? 80 : 78, large ? 154 : 140, large ? 18 : 13);

    view->bar = lv_bar_create(parent);
    lv_obj_set_pos(view->bar, TINYOS_CONTENT_X, large ? 104 : 102);
    lv_obj_set_size(view->bar, TINYOS_CONTENT_W, large ? 5 : 4);
    lv_bar_set_range(view->bar, 0, 1000);
    lv_obj_set_style_bg_color(view->bar, lv_color_hex(TINYOS_COLOR_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(view->bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(view->bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(view->bar, lv_color_hex(TINYOS_COLOR_CYAN),
                              LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(view->bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(view->bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_anim_duration(view->bar, 450, 0);

    view->position = tinyos_label(parent, "0:00", &lv_font_montserrat_10,
                                  TINYOS_COLOR_MUTED, TINYOS_CONTENT_X,
                                  large ? 112 : 108, 60, large ? 18 : 12);
    view->duration = tinyos_label(parent, "0:00", &lv_font_montserrat_10,
                                  TINYOS_COLOR_MUTED, 166, large ? 112 : 108, 60,
                                  large ? 18 : 12);
    lv_obj_set_style_text_align(view->duration, LV_TEXT_ALIGN_RIGHT, 0);

    int prev_y = 132;
    int play_size = large ? 46 : 44;
    int play_y = 124;
    lv_obj_t *prev = icon_button(parent, large ? 62 : 66, prev_y, large ? 30 : 28,
                                 large ? 16 : 16, LV_SYMBOL_PREV,
                                 TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_MUTED,
                                 prev_cb, view);
    lv_obj_t *play = icon_button(parent, large ? 97 : 98, play_y, play_size,
                                 large ? 24 : 22, LV_SYMBOL_PLAY, TINYOS_COLOR_CYAN,
                                 TINYOS_COLOR_ON_ACCENT, play_cb, view);
    lv_obj_t *next = icon_button(parent, large ? 148 : 146, prev_y,
                                 large ? 30 : 28, large ? 16 : 16, LV_SYMBOL_NEXT,
                                 TINYOS_COLOR_SURFACE_2, TINYOS_COLOR_MUTED,
                                 next_cb, view);

    view->play_icon = lv_obj_get_child(play, 0);

    /* The play button is accent-filled, so the normal accent focus border is
     * invisible. Give this one a foreground-coloured ring. */
    tinyos_focus_on_accent(play);
    tinyos_shell_add_focus(ctx, play);
    tinyos_shell_add_focus(ctx, prev);
    tinyos_shell_add_focus(ctx, next);

    tinyos_reveal(art, 0);
    tinyos_reveal(view->title, 40);
    tinyos_reveal(play, 80);
    return parent;
}

const tinyos_module tinyos_module_music = {
    .id = "music",
    .name = "Music",
    .subtitle = "WAV player",
    .icon = LV_SYMBOL_AUDIO,
    .create = music_create,
};
