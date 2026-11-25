#include "app.h"

#include "core/log.h"
#include "platform.h"
#include "ui/shell.h"
#include "ui/theme.h"

#include <string.h>

static tinyos_ctx application;

tinyos_ctx *tinyos_ctx_get(void)
{
    return &application;
}

int tinyos_app_init(void)
{
    memset(&application, 0, sizeof(application));

    lv_init();
    lv_tick_set_cb(tinyos_tick_ms);

    if (tinyos_platform_init() != 0) {
        TINYOS_LOGE("platform initialisation failed");
        return -1;
    }

    application.display = tinyos_display_create();
    if (!application.display) {
        TINYOS_LOGE("no display available");
        return -1;
    }

    application.input = tinyos_input_create();
    tinyos_theme_init();

    lv_theme_t *theme = lv_theme_default_init(application.display,
                                              lv_color_hex(TINYOS_COLOR_CYAN),
                                              lv_color_hex(TINYOS_COLOR_VIOLET), true,
                                              &lv_font_montserrat_14);
    lv_display_set_theme(application.display, theme);

    tinyos_settings_defaults(&application.settings);
    tinyos_settings_load(&application.settings, TINYOS_SETTINGS_PATH);

    application.audio = tinyos_audio_create();
    if (application.audio) {
        tinyos_audio_set_volume(application.audio, application.settings.volume);
    } else {
        TINYOS_LOGW("audio service unavailable");
    }

    tinyos_brightness_set(application.settings.brightness);
    tinyos_performance_set(application.settings.performance);

    tinyos_shell_init(&application);
    application.running = true;

    TINYOS_LOGI("TinyOS starting on %dx%d", 240, 240);
    return 0;
}

int tinyos_app_run(void)
{
    if (tinyos_app_init() != 0)
        return 1;

    while (application.running) {
        lv_timer_handler();
        tinyos_platform_delay_ms(5);
    }

    tinyos_app_shutdown();
    return 0;
}

void tinyos_app_shutdown(void)
{
    application.running = false;
    if (application.audio) {
        tinyos_audio_destroy(application.audio);
        application.audio = NULL;
    }
    tinyos_platform_shutdown();
}
