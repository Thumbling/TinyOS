#include "emulator.h"

#include "session.h"

#include "../app.h"
#include "../core/audio.h"
#include "../core/log.h"
#include "../core/settings.h"
#include "../platform.h"

#include <mgba/core/blip_buf.h>
#include <mgba/core/core.h>

#include <stdio.h>
#include <stdlib.h>

#define GAME_AUDIO_BUFFER 2048

static int16_t left_samples[GAME_AUDIO_BUFFER];
static int16_t right_samples[GAME_AUDIO_BUFFER];
static int16_t stereo_samples[GAME_AUDIO_BUFFER * 2];

static void pump_audio(struct mCore *core, bool audio_available, int volume)
{
    if (!core || !core->getAudioChannel)
        return;

    struct blip_t *left = core->getAudioChannel(core, 0);
    struct blip_t *right = core->getAudioChannel(core, 1);
    if (!left || !right)
        return;

    while (blip_samples_avail(left) > 0) {
        int count = blip_samples_avail(left);
        if (count > GAME_AUDIO_BUFFER)
            count = GAME_AUDIO_BUFFER;
        int got_left = blip_read_samples(left, left_samples, count, 0);
        int got_right = blip_read_samples(right, right_samples, count, 0);
        int got = got_left < got_right ? got_left : got_right;
        if (got <= 0)
            break;

        for (int i = 0; i < got; i++) {
            int32_t l = (int32_t)left_samples[i] * volume / 100;
            int32_t r = (int32_t)right_samples[i] * volume / 100;
            if (l > 32767) l = 32767;
            if (l < -32768) l = -32768;
            if (r > 32767) r = 32767;
            if (r < -32768) r = -32768;
            stereo_samples[i * 2] = (int16_t)l;
            stereo_samples[i * 2 + 1] = (int16_t)r;
        }
        if (audio_available)
            tinyos_game_audio_write(stereo_samples, got);
    }
}

int tinyos_emulator_run(const tinyos_rom_entry *rom)
{
    tinyos_emulator_session *session = tinyos_emulator_session_open(rom);
    if (!session)
        return -1;

    tinyos_ctx *ctx = tinyos_ctx_get();
    if (ctx && ctx->audio)
        tinyos_audio_stop(ctx->audio);
    int previous_performance = ctx ? ctx->settings.performance : TINYOS_PERF_BALANCED;
    tinyos_performance_set(TINYOS_PERF_TURBO);
    tinyos_game_input_begin();
    bool audio_available = tinyos_game_audio_begin(TINYOS_GAME_SAMPLE_RATE);

    TINYOS_LOGI("game: %s (%s), audio %s", rom->name,
                tinyos_rom_system_tag(rom->system),
                audio_available ? "on" : "unavailable");

    int frame_limit = -1;
#ifdef TINYOS_SIM
    const char *frame_limit_env = getenv("TINYOS_SIM_GAME_FRAMES");
    if (frame_limit_env && frame_limit_env[0])
        frame_limit = atoi(frame_limit_env);
#endif
    int frames_done = 0;
    tinyos_game_exit_gesture gesture;
    tinyos_game_exit_gesture_init(&gesture);
    unsigned pacing_phase = 0;
    uint32_t next_frame = tinyos_tick_ms();

    while (true) {
        if (frame_limit >= 0 && frames_done >= frame_limit)
            break;
        uint32_t keys = tinyos_game_input_poll();
        if (keys & TINYOS_GAME_KEY_EXIT)
            break;

        uint32_t now = tinyos_tick_ms();
        if (tinyos_game_exit_gesture_update(&gesture, keys, now))
            break;

        if (!tinyos_emulator_session_run_frame(session, keys))
            break;
        tinyos_game_present_rgb565(tinyos_emulator_session_panel(session),
                                   TINYOS_GAME_PANEL_SIZE,
                                   TINYOS_GAME_PANEL_SIZE);

        pump_audio(tinyos_emulator_session_core(session), audio_available,
                   ctx ? ctx->settings.volume : 62);
        frames_done++;

        /* Audio remains the long-term clock. The 16.7427 ms millisecond
         * pattern keeps the video cadence aligned with 59.7275 Hz whenever
         * audio is unavailable or temporarily silent. */
        if (frame_limit < 0) {
            next_frame += tinyos_game_pacing_delay_ms(&pacing_phase);
            int32_t wait = (int32_t)(next_frame - tinyos_tick_ms());
            if (wait > 0)
                tinyos_platform_delay_ms(wait);
            else
                next_frame = tinyos_tick_ms();
        } else {
            /* The simulator's UI clock is advanced by its frame loop, not by
             * wall time. A bounded smoke run must not wait on a clock that
             * nobody advances. */
            next_frame = tinyos_tick_ms();
        }
    }

    tinyos_game_input_end();
    tinyos_game_audio_end();
    tinyos_emulator_session_close(session, true);
    tinyos_performance_set(previous_performance);
    TINYOS_LOGI("game: returned to library");
    return 0;
}

const char *tinyos_emulator_last_error(void)
{
    return tinyos_emulator_session_last_error();
}
