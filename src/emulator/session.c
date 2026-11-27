#include "session.h"

#include "game_keys.h"
#include "../core/log.h"

#include <mgba/core/config.h>
#include <mgba/core/core.h>
#include <mgba/gb/core.h>
#include <mgba/gba/core.h>
#include <mgba/internal/gb/input.h>
#include <mgba/internal/gba/input.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define GAME_AUDIO_BUFFER 2048

struct tinyos_emulator_session {
    struct mCore *core;
    enum mPlatform platform;
    bool config_ready;
    color_t *video;
    unsigned core_width;
    unsigned core_height;
    tinyos_emulator_geometry geometry;
    uint16_t panel[TINYOS_GAME_PANEL_SIZE * TINYOS_GAME_PANEL_SIZE];
    char save_path[TINYOS_ROM_PATH_MAX];
};

static char last_error[160];

static uint16_t rgb565(color_t pixel)
{
    /* mGBA's non-planar color_t is 0x00BBGGRR. The panel is little-endian
     * RGB565: 0xF800 red, 0x07E0 green, 0x001F blue. */
    unsigned r = pixel & 0xFFu;
    unsigned g = (pixel >> 8) & 0xFFu;
    unsigned b = (pixel >> 16) & 0xFFu;
    return (uint16_t)(((r & 0xF8u) << 8) | ((g & 0xFCu) << 3) |
                      ((b & 0xF8u) >> 3));
}

static void compose_gba(tinyos_emulator_session *session,
                        const color_t *pixels, size_t stride,
                        unsigned width, unsigned height)
{
    if (!pixels)
        return;
    if (width > TINYOS_GAME_PANEL_SIZE)
        width = TINYOS_GAME_PANEL_SIZE;
    if (height > 160)
        height = 160;

    for (unsigned y = 0; y < height; y++) {
        const color_t *row = pixels + (size_t)y * stride;
        uint16_t *out = &session->panel[(y + TINYOS_GAME_BARS_GBA) *
                                        TINYOS_GAME_PANEL_SIZE];
        for (unsigned x = 0; x < width; x++)
            out[x] = rgb565(row[x]);
    }
}

static void compose_gb(tinyos_emulator_session *session,
                       const color_t *pixels, size_t stride,
                       unsigned width, unsigned height)
{
    if (!pixels)
        return;

    /* A loaded DMG/CGB normally asks for 160x144. Some models report the
     * 256x224 border image; crop the border before scaling. */
    unsigned source_x = 0;
    unsigned source_y = 0;
    if (width == 256 && height == 224) {
        source_x = 48;
        source_y = 40;
        width = 160;
        height = 144;
    }
    if (width > 160)
        width = 160;
    if (height > 144)
        height = 144;

    /* 3:2 nearest-neighbour fills 240x216 without a filtered, blurry image.
     * GBA uses its native 240x160 and only needs top/bottom letterboxing. */
    for (unsigned y = 0; y < 216; y++) {
        unsigned sy = source_y + (y * 2u) / 3u;
        if (sy >= source_y + height)
            sy = source_y + height - 1;
        const color_t *row = pixels + (size_t)sy * stride;
        uint16_t *out = &session->panel[(y + TINYOS_GAME_BARS_GB) *
                                        TINYOS_GAME_PANEL_SIZE];
        for (unsigned x = 0; x < TINYOS_GAME_PANEL_SIZE; x++) {
            unsigned sx = source_x + (x * 2u) / 3u;
            if (sx >= source_x + width)
                sx = source_x + width - 1;
            out[x] = rgb565(row[sx]);
        }
    }
}

static void save_path_for(const char *rom_path, char *destination, size_t size)
{
    if (!destination || size == 0)
        return;
    snprintf(destination, size, "%s", rom_path);
    char *slash = strrchr(destination, '/');
    char *backslash = strrchr(destination, '\\');
    if (backslash && (!slash || backslash > slash))
        slash = backslash;
    char *dot = strrchr(destination, '.');
    if (dot && (!slash || dot > slash))
        *dot = '\0';
    size_t used = strlen(destination);
    if (used + 5 < size)
        snprintf(destination + used, size - used, ".sav");
}

static void write_save_file(const char *path, const void *save, size_t size)
{
    if (!save || size == 0)
        return;

    /* mGBA keeps its save VFile open and mapped. On Windows a second fopen of
     * that same path can fail with EINVAL, so the core is deinitialized before
     * this function is called. */
    FILE *file = fopen(path, "wb");
    if (file) {
        size_t written = fwrite(save, 1, size, file);
        if (fclose(file) != 0 || written != size)
            TINYOS_LOGW("could not finish save %s", path);
        else
            TINYOS_LOGI("saved %zu bytes to %s", size, path);
    } else {
        TINYOS_LOGW("cannot write save %s: %s", path, strerror(errno));
    }
}

static void session_destroy(tinyos_emulator_session *session)
{
    if (!session)
        return;
    if (session->core) {
        if (session->config_ready)
            mCoreConfigDeinit(&session->core->config);
        session->core->deinit(session->core);
    }
    free(session->video);
    free(session);
}

tinyos_emulator_session *tinyos_emulator_session_open(const tinyos_rom_entry *rom)
{
    last_error[0] = '\0';
    if (!rom || !rom->path[0]) {
        snprintf(last_error, sizeof(last_error), "No ROM selected.");
        return NULL;
    }

    tinyos_emulator_session *session = calloc(1, sizeof(*session));
    if (!session) {
        snprintf(last_error, sizeof(last_error), "Not enough memory for the emulator session.");
        return NULL;
    }

    enum mPlatform platform = rom->system == TINYOS_ROM_GBA ? mPLATFORM_GBA : mPLATFORM_GB;
    session->platform = platform;
    session->core = mCoreCreate(platform);
    if (!session->core) {
        snprintf(last_error, sizeof(last_error), "No emulator core for this system.");
        session_destroy(session);
        return NULL;
    }
    if (!session->core->init(session->core)) {
        snprintf(last_error, sizeof(last_error), "Could not initialise the emulator core.");
        session_destroy(session);
        return NULL;
    }

    mCoreInitConfig(session->core, "tinyos");
    session->config_ready = true;
    mCoreConfigSetDefaultIntValue(&session->core->config, "sampleRate",
                                  TINYOS_GAME_SAMPLE_RATE);
    mCoreConfigSetDefaultIntValue(&session->core->config, "audioBuffers",
                                  GAME_AUDIO_BUFFER);
    mCoreConfigSetDefaultIntValue(&session->core->config, "frameskip", 0);
    mCoreConfigSetDefaultIntValue(&session->core->config, "logLevel", 0);
    mCoreLoadForeignConfig(session->core, &session->core->config);

    if (!mCoreLoadFile(session->core, rom->path)) {
        snprintf(last_error, sizeof(last_error), "This file is not a valid %s ROM.",
                 tinyos_rom_system_name(rom->system));
        session_destroy(session);
        return NULL;
    }

    save_path_for(rom->path, session->save_path, sizeof(session->save_path));
    struct stat save_info;
    bool save_loaded = false;
    if (stat(session->save_path, &save_info) == 0 && save_info.st_size > 0)
        save_loaded = mCoreLoadSaveFile(session->core, session->save_path, false);
    if (!save_loaded) {
        /* GB/GBC only allocate and map their cartridge SRAM while loading a
         * save VFile. A first run without a .sav would otherwise silently
         * route every $A000 write to the MBC and never create a save. GBA
         * needs the same empty VFile to detect SRAM on its first write. */
        mCoreLoadSaveFile(session->core, session->save_path, false);
    }

    /* Reset before asking for dimensions: a GB/CGB core only resolves the
     * cartridge model (and therefore 160x144 vs. an SGB border) during
     * GBReset. Asking earlier returns 256x224 and mis-sizes the frame. */
    session->core->reset(session->core);
    session->core->desiredVideoDimensions(session->core, &session->core_width,
                                           &session->core_height);
    session->video = calloc((size_t)session->core_width * session->core_height,
                            sizeof(*session->video));
    if (!session->video) {
        snprintf(last_error, sizeof(last_error),
                 "Not enough memory for the video buffer.");
        session_destroy(session);
        return NULL;
    }
    session->core->setVideoBuffer(session->core, session->video,
                                  session->core_width);
    session->core->setAudioBufferSize(session->core, GAME_AUDIO_BUFFER);
    session->core->reset(session->core);

    bool gba = rom->system == TINYOS_ROM_GBA;
    session->geometry = (tinyos_emulator_geometry){
        .core_width = session->core_width,
        .core_height = session->core_height,
        .panel_width = TINYOS_GAME_PANEL_SIZE,
        .panel_height = TINYOS_GAME_PANEL_SIZE,
        .image_x = 0,
        .image_y = gba ? TINYOS_GAME_BARS_GBA : TINYOS_GAME_BARS_GB,
        .image_width = TINYOS_GAME_PANEL_SIZE,
        .image_height = gba ? 160 : 216,
    };

    TINYOS_LOGI("game: %s (%s), %ux%u", rom->name,
                tinyos_rom_system_tag(rom->system), session->core_width,
                session->core_height);
    return session;
}

bool tinyos_emulator_session_run_frame(tinyos_emulator_session *session, uint32_t keys)
{
    if (!session || !session->core)
        return false;

    session->core->setKeys(session->core,
                           tinyos_emulator_controller_keys(keys,
                               session->platform == mPLATFORM_GBA
                                   ? TINYOS_ROM_GBA : TINYOS_ROM_GB));
    session->core->runFrame(session->core);

    const void *pixels = NULL;
    size_t stride = 0;
    session->core->getPixels(session->core, &pixels, &stride);
    memset(session->panel, 0, sizeof(session->panel));
    if (session->platform == mPLATFORM_GBA)
        compose_gba(session, pixels, stride, session->core_width,
                    session->core_height);
    else
        compose_gb(session, pixels, stride, session->core_width,
                   session->core_height);
    return true;
}

const uint16_t *tinyos_emulator_session_panel(const tinyos_emulator_session *session)
{
    return session ? session->panel : NULL;
}

const tinyos_emulator_geometry *tinyos_emulator_session_geometry(
    const tinyos_emulator_session *session)
{
    return session ? &session->geometry : NULL;
}

const char *tinyos_emulator_session_save_path(const tinyos_emulator_session *session)
{
    return session ? session->save_path : NULL;
}

size_t tinyos_emulator_session_clone_save(tinyos_emulator_session *session, void **data)
{
    if (data)
        *data = NULL;
    if (!session || !session->core || !session->core->savedataClone)
        return 0;

    void *save = NULL;
    size_t size = session->core->savedataClone(session->core, &save);
    if (data)
        *data = save;
    return size;
}

const char *tinyos_emulator_session_last_error(void)
{
    return last_error;
}

struct mCore *tinyos_emulator_session_core(tinyos_emulator_session *session)
{
    return session ? session->core : NULL;
}

void tinyos_emulator_session_close(tinyos_emulator_session *session, bool save_on_close)
{
    if (!session)
        return;

    void *save = NULL;
    size_t save_size = 0;
    char save_path[TINYOS_ROM_PATH_MAX];
    snprintf(save_path, sizeof(save_path), "%s", session->save_path);
    if (save_on_close && session->core && session->core->savedataClone) {
        save_size = session->core->savedataClone(session->core, &save);
        if (!save || save_size == 0) {
            free(save);
            save = NULL;
            save_size = 0;
        }
    }

    session_destroy(session);
    write_save_file(save_path, save, save_size);
    free(save);
}

uint32_t tinyos_emulator_controller_keys(uint32_t keys, tinyos_rom_system system)
{
    uint32_t result = 0;
    if (system == TINYOS_ROM_GBA) {
        if (keys & TINYOS_GAME_KEY_A)      result |= 1u << GBA_KEY_A;
        if (keys & TINYOS_GAME_KEY_B)      result |= 1u << GBA_KEY_B;
        if (keys & TINYOS_GAME_KEY_SELECT) result |= 1u << GBA_KEY_SELECT;
        if (keys & TINYOS_GAME_KEY_START)  result |= 1u << GBA_KEY_START;
        if (keys & TINYOS_GAME_KEY_RIGHT)  result |= 1u << GBA_KEY_RIGHT;
        if (keys & TINYOS_GAME_KEY_LEFT)   result |= 1u << GBA_KEY_LEFT;
        if (keys & TINYOS_GAME_KEY_UP)     result |= 1u << GBA_KEY_UP;
        if (keys & TINYOS_GAME_KEY_DOWN)   result |= 1u << GBA_KEY_DOWN;
        if (keys & TINYOS_GAME_KEY_R)      result |= 1u << GBA_KEY_R;
        if (keys & TINYOS_GAME_KEY_L)      result |= 1u << GBA_KEY_L;
    } else {
        if (keys & TINYOS_GAME_KEY_A)      result |= 1u << GB_KEY_A;
        if (keys & TINYOS_GAME_KEY_B)      result |= 1u << GB_KEY_B;
        if (keys & TINYOS_GAME_KEY_SELECT) result |= 1u << GB_KEY_SELECT;
        if (keys & TINYOS_GAME_KEY_START)  result |= 1u << GB_KEY_START;
        if (keys & TINYOS_GAME_KEY_RIGHT)  result |= 1u << GB_KEY_RIGHT;
        if (keys & TINYOS_GAME_KEY_LEFT)   result |= 1u << GB_KEY_LEFT;
        if (keys & TINYOS_GAME_KEY_UP)     result |= 1u << GB_KEY_UP;
        if (keys & TINYOS_GAME_KEY_DOWN)   result |= 1u << GB_KEY_DOWN;
    }
    return result;
}

void tinyos_game_exit_gesture_init(tinyos_game_exit_gesture *gesture)
{
    if (gesture)
        memset(gesture, 0, sizeof(*gesture));
}

bool tinyos_game_exit_gesture_update(tinyos_game_exit_gesture *gesture,
                                     uint32_t keys, uint32_t now_ms)
{
    if (!gesture || gesture->fired)
        return false;

    const uint32_t chord = TINYOS_GAME_KEY_X | TINYOS_GAME_KEY_Y;
    if ((keys & chord) != chord) {
        gesture->armed = false;
        return false;
    }

    if (!gesture->armed) {
        gesture->armed = true;
        gesture->started_ms = now_ms;
        return false;
    }

    if ((uint32_t)(now_ms - gesture->started_ms) < TINYOS_GAME_EXIT_CHORD_MS)
        return false;

    gesture->fired = true;
    return true;
}

uint32_t tinyos_game_pacing_delay_ms(unsigned *phase)
{
    if (!phase)
        return 17;
    *phase += TINYOS_GAME_PACING_16MS;
    if (*phase >= TINYOS_GAME_PACING_WINDOW) {
        *phase -= TINYOS_GAME_PACING_WINDOW;
        return 16;
    }
    return 17;
}

uint64_t tinyos_game_audio_samples_for_frames(uint32_t frames)
{
    /* Round the ideal sample position so a long run has no cumulative drift. */
    uint64_t numerator = (uint64_t)TINYOS_GAME_SAMPLE_RATE * frames *
                         TINYOS_GAME_FPS_DEN;
    return (numerator + TINYOS_GAME_FPS_NUM / 2) / TINYOS_GAME_FPS_NUM;
}
