#include "../src/app.h"
#include "../src/core/key_repeat.h"
#include "../src/modules/music.h"
#include "../src/platform.h"
#include "../src/ui/fonts.h"
#include "../src/ui/shell.h"
#include "../src/ui/theme.h"
#include "../src/ui/wallpaper.h"

#include "png.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *program)
{
    fprintf(stderr, "usage: %s [--screen home|apps|settings|music|system|about|"
                    "appearance|themes|wallpapers|typeface] [--out file.png] "
                    "[--theme N] [--font N] [--wallpaper N] [--frames N] "
                    "[--text 100|115|130] [--state normal|empty|loading|error] "
                    "[--down N] [--enter] [--keytest]\n", program);
}

static int keytest_clicks;

static void keytest_click_cb(lv_event_t *event)
{
    (void)event;
    keytest_clicks++;
}

static void keytest_frame(void)
{
    tinyos_host_tick_advance(16);
    lv_timer_handler();
}

static int run_keytest(tinyos_ctx *ctx)
{
    /* 1. A held through the real host indev must click exactly once. */
    lv_obj_t *button = lv_button_create(ctx->content);
    lv_obj_set_size(button, 80, 24);
    lv_obj_set_pos(button, 80, 70);
    lv_obj_add_event_cb(button, keytest_click_cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(ctx->group, button);
    lv_group_focus_obj(button);

    tinyos_host_input_down(LV_KEY_ENTER);
    for (int frame = 0; frame < 45; frame++)
        keytest_frame();
    tinyos_host_input_up(LV_KEY_ENTER);
    keytest_frame();
    int ui_clicks = keytest_clicks;

    /* 2. The shared policy: A emits one event, directions repeat and stop. */
    tinyos_key_repeat_reset();
    tinyos_key_repeat_down(LV_KEY_ENTER, 0);
    int action_events = 0;
    for (uint32_t t = 0; t <= 600; t += 16) {
        if (tinyos_key_repeat_poll(t) != 0)
            action_events++;
    }
    tinyos_key_repeat_up(LV_KEY_ENTER, 600);

    tinyos_key_repeat_reset();
    tinyos_key_repeat_down(LV_KEY_RIGHT, 0);
    int repeats = 0;
    for (uint32_t t = 0; t <= 480; t += 16) {
        if (tinyos_key_repeat_poll(t) != 0)
            repeats++;
    }
    tinyos_key_repeat_up(LV_KEY_RIGHT, 500);
    int after_release = 0;
    for (uint32_t t = 500; t <= 900; t += 16) {
        if (tinyos_key_repeat_poll(t) != 0)
            after_release++;
    }

    bool pass = ui_clicks == 1 && action_events == 1 && repeats >= 3
                && after_release == 0;
    printf("keytest: held A -> %d click / %d event, d-pad repeats %d, after release %d: %s\n",
           ui_clicks, action_events, repeats, after_release, pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}

int main(int argc, char **argv)
{
    const char *screen = "home";
    const char *output = "build/home.png";
    int theme = -1;
    int font = -1;
    int wallpaper = -1;
    int text = -1;
    const char *state = NULL;
    int downs = 0;
    bool enter = false;
    int frames = 90;
    bool keytest = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--screen") == 0 && i + 1 < argc) {
            screen = argv[++i];
        } else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc) {
            output = argv[++i];
        } else if (strcmp(argv[i], "--theme") == 0 && i + 1 < argc) {
            theme = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--font") == 0 && i + 1 < argc) {
            font = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--wallpaper") == 0 && i + 1 < argc) {
            wallpaper = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--text") == 0 && i + 1 < argc) {
            text = atoi(argv[++i]);
            if (text != 100 && text != 115 && text != 130)
                text = 100;
        } else if (strcmp(argv[i], "--state") == 0 && i + 1 < argc) {
            state = argv[++i];
        } else if (strcmp(argv[i], "--down") == 0 && i + 1 < argc) {
            downs = atoi(argv[++i]);
            if (downs < 0)
                downs = 0;
        } else if (strcmp(argv[i], "--enter") == 0) {
            enter = true;
        } else if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc) {
            frames = atoi(argv[++i]);
            if (frames < 1)
                frames = 1;
        } else if (strcmp(argv[i], "--keytest") == 0) {
            keytest = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    if (tinyos_app_init() != 0) {
        fprintf(stderr, "failed to initialise TinyOS\n");
        return 1;
    }

    tinyos_ctx *ctx = tinyos_ctx_get();
    if (theme >= 0) {
        ctx->settings.theme = theme;
        tinyos_theme_apply(theme);
    }
    if (font >= 0) {
        ctx->settings.font_family = font;
        tinyos_fonts_select(font);
    }
    if (text >= 0)
        ctx->settings.text_scale = text;
    if (state) {
        if (strcmp(state, "empty") == 0)
            tinyos_music_force_state(TINYOS_MUSIC_EMPTY);
        else if (strcmp(state, "loading") == 0)
            tinyos_music_force_state(TINYOS_MUSIC_LOADING);
        else if (strcmp(state, "error") == 0)
            tinyos_music_force_state(TINYOS_MUSIC_ERROR);
        else
            tinyos_music_force_state(TINYOS_MUSIC_NORMAL);
    }
    if (wallpaper >= 0) {
        ctx->settings.wallpaper = tinyos_wallpaper_clamp_style(wallpaper);
        tinyos_shell_set_wallpaper(ctx, ctx->settings.wallpaper,
                                   ctx->settings.reduce_motion ? 0 : ctx->settings.wallpaper_speed);
    }
    if (theme >= 0 || font >= 0 || text >= 0)
        tinyos_shell_reload(ctx, screen);
    else
        tinyos_shell_show(ctx, screen);

    if (keytest) {
        int result = run_keytest(ctx);
        tinyos_app_shutdown();
        return result;
    }

    /* The screen needs one layout pass before geometric focus navigation has
     * real coordinates; a device naturally has those frames before a key. */
    for (int frame = 0; frame < 4; frame++) {
        tinyos_host_tick_advance(16);
        lv_timer_handler();
    }

    /* Drives the real host indev through the focus ring, so the focus ring and
     * scroll position in a large-text screenshot are not hand-placed. */
    for (int i = 0; i < downs; i++) {
        tinyos_host_input_key(LV_KEY_DOWN);
        for (int frame = 0; frame < 8; frame++) {
            tinyos_host_tick_advance(16);
            lv_timer_handler();
        }
    }

    if (enter) {
        tinyos_host_input_key(LV_KEY_ENTER);
        for (int frame = 0; frame < 20; frame++) {
            tinyos_host_tick_advance(16);
            lv_timer_handler();
        }
    }

    for (int frame = 0; frame < frames; frame++) {
        tinyos_host_tick_advance(16);
        lv_timer_handler();
    }
    lv_refr_now(ctx->display);

    if (tinyos_png_write_rgb565(output, tinyos_host_framebuffer(), 240, 240) != 0) {
        fprintf(stderr, "failed to write %s\n", output);
        tinyos_app_shutdown();
        return 1;
    }

    printf("rendered %s -> %s\n", screen, output);
    tinyos_app_shutdown();
    return 0;
}
