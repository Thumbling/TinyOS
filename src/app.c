#include "app.h"

#include "core/log.h"
#include "platform.h"
#include "ui/fonts.h"
#include "ui/shell.h"
#include "ui/theme.h"

#include <string.h>

#ifndef TINYOS_SIM
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
static int instance_lock_fd = -1;
#endif

static tinyos_ctx application;

tinyos_ctx *tinyos_ctx_get(void)
{
    return &application;
}

/* Only one TinyOS process may own the panel. fbcon is detached by
 * tinyos-run.sh before this process starts, but a second UI started manually
 * over the serial console would otherwise become a second writer to /dev/fb0. */
static int take_instance_lock(void)
{
#ifndef TINYOS_SIM
    int fd = open("/tmp/tinyos-ui.lock", O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (fd < 0) {
        TINYOS_LOGE("cannot open UI lock: %s", strerror(errno));
        return -1;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLK, &lock) != 0) {
        TINYOS_LOGE("another TinyOS UI already owns the panel: %s", strerror(errno));
        close(fd);
        return -1;
    }

    instance_lock_fd = fd;
#endif
    return 0;
}

static void release_instance_lock(void)
{
#ifndef TINYOS_SIM
    if (instance_lock_fd >= 0) {
        close(instance_lock_fd);
        instance_lock_fd = -1;
    }
#endif
}

int tinyos_app_init(void)
{
    memset(&application, 0, sizeof(application));

    if (take_instance_lock() != 0)
        return -1;

    lv_init();
    lv_tick_set_cb(tinyos_tick_ms);

    if (tinyos_platform_init() != 0) {
        TINYOS_LOGE("platform initialisation failed");
        release_instance_lock();
        return -1;
    }

    application.display = tinyos_display_create();
    if (!application.display) {
        TINYOS_LOGE("no display available");
        release_instance_lock();
        return -1;
    }

    application.input = tinyos_input_create();

    tinyos_settings_defaults(&application.settings);
    tinyos_settings_load(&application.settings, TINYOS_SETTINGS_PATH);

    tinyos_fonts_select(application.settings.font_family);
    tinyos_theme_apply(application.settings.theme);

    lv_theme_t *theme = lv_theme_default_init(application.display,
                                              lv_color_hex(TINYOS_COLOR_CYAN),
                                              lv_color_hex(TINYOS_COLOR_VIOLET), true,
                                              tinyos_font_ui(14));
    lv_display_set_theme(application.display, theme);

    application.audio = tinyos_audio_create();
    if (application.audio) {
        tinyos_audio_set_volume(application.audio, application.settings.volume);
    } else {
        TINYOS_LOGW("audio service unavailable");
    }

    tinyos_brightness_set(application.settings.brightness);
    tinyos_performance_set(application.settings.performance);

    tinyos_shell_init(&application);

    /* Partial mode invalidates dirty rectangles only. Force the whole active
     * screen through one synchronous refresh before the event loop: the
     * display backend has already cleared all mapped framebuffer bytes, and
     * this full frame replaces any console pixels a previous writer left in
     * areas the normal first layout pass might never invalidate. */
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(application.display);
    TINYOS_LOGI("first frame committed: full %dx%d panel", 240, 240);

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
    release_instance_lock();
}
