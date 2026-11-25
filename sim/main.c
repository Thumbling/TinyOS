#include "../src/app.h"
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
                    "[--theme N] [--font N] [--wallpaper N]\n", program);
}

int main(int argc, char **argv)
{
    const char *screen = "home";
    const char *output = "build/home.png";
    int theme = -1;
    int font = -1;
    int wallpaper = -1;

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
    if (wallpaper >= 0) {
        ctx->settings.wallpaper = tinyos_wallpaper_clamp_style(wallpaper);
        tinyos_shell_set_wallpaper(ctx, ctx->settings.wallpaper,
                                   ctx->settings.reduce_motion ? 0 : ctx->settings.wallpaper_speed);
    }
    if (theme >= 0 || font >= 0)
        tinyos_shell_reload(ctx, screen);
    else
        tinyos_shell_show(ctx, screen);

    for (int frame = 0; frame < 90; frame++) {
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
