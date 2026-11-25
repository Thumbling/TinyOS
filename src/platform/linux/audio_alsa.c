/*
 * TinyOS ALSA WAV player.
 *
 * Tracks are plain PCM16 WAV files so the player stays dependency-light and
 * predictable on the V3s. A dedicated playback thread feeds ALSA; the UI only
 * reads atomically guarded state and never blocks on the codec.
 */
#include "../../platform.h"
#include "../../core/audio.h"
#include "../../core/log.h"
#include "../../core/util.h"

#include <alsa/asoundlib.h>
#include <ctype.h>
#include <dirent.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#define MAX_TRACKS 128
#define PLAY_FRAMES 1024
#define OUTPUT_CHANNELS 2

typedef struct {
    uint16_t format;
    uint16_t channels;
    uint16_t bits;
    uint32_t rate;
    uint32_t data_offset;
    uint32_t data_size;
} wav_info;

struct tinyos_audio {
    tinyos_track tracks[MAX_TRACKS];
    int count;
    int current;
    int volume;
    int position_ms;
    int duration_ms;
    bool playing;
    bool thread_running;
    bool stop_requested;
    pthread_t thread;
    pthread_mutex_t lock;
};

static uint16_t read_u16(const uint8_t *p)
{
    return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static bool wav_info_read(const char *path, wav_info *info)
{
    memset(info, 0, sizeof(*info));

    FILE *file = fopen(path, "rb");
    if (!file)
        return false;

    uint8_t header[12];
    if (fread(header, 1, sizeof(header), file) != sizeof(header) ||
        memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0) {
        fclose(file);
        return false;
    }

    bool got_format = false;
    bool got_data = false;
    while (!got_data) {
        uint8_t chunk[8];
        if (fread(chunk, 1, sizeof(chunk), file) != sizeof(chunk))
            break;

        uint32_t size = read_u32(chunk + 4);
        long next = ftell(file) + (long)size + (long)(size & 1u);

        if (memcmp(chunk, "fmt ", 4) == 0 && size >= 16) {
            uint8_t format[16];
            if (fread(format, 1, sizeof(format), file) != sizeof(format))
                break;
            info->format = read_u16(format);
            info->channels = read_u16(format + 2);
            info->rate = read_u32(format + 4);
            info->bits = read_u16(format + 14);
            got_format = true;
        } else if (memcmp(chunk, "data", 4) == 0) {
            info->data_offset = (uint32_t)ftell(file);
            info->data_size = size;
            got_data = true;
            break;
        }

        if (fseek(file, next, SEEK_SET) != 0)
            break;
    }

    if (got_format && got_data) {
        long end = ftell(file);
        if (fseek(file, 0, SEEK_END) == 0) {
            long file_size = ftell(file);
            if (file_size > end && info->data_size > (uint32_t)(file_size - (long)info->data_offset))
                info->data_size = (uint32_t)(file_size - (long)info->data_offset);
        }
    }
    fclose(file);

    return got_format && got_data && info->format == 1 &&
           (info->channels == 1 || info->channels == 2) &&
           info->bits == 16 && info->rate >= 8000 && info->data_size > 0;
}

static int wav_duration_ms(const wav_info *info)
{
    uint64_t bytes_per_second = (uint64_t)info->rate * info->channels * 2u;
    if (bytes_per_second == 0)
        return 0;
    return (int)((uint64_t)info->data_size * 1000u / bytes_per_second);
}

static bool has_wav_suffix(const char *name)
{
    size_t length = strlen(name);
    return length > 4 && strcasecmp(name + length - 4, ".wav") == 0;
}

static void track_title(char *destination, size_t size, const char *path)
{
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    snprintf(destination, size, "%s", base);

    char *dot = strrchr(destination, '.');
    if (dot)
        *dot = '\0';
    for (char *p = destination; *p; p++) {
        if (*p == '_')
            *p = ' ';
    }
}

static void scan_directory(tinyos_audio *audio, const char *directory)
{
    DIR *dir = opendir(directory);
    if (!dir)
        return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' || !has_wav_suffix(entry->d_name))
            continue;

        char path[256];
        if (snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name) >= (int)sizeof(path))
            continue;
        if (audio->count >= MAX_TRACKS)
            break;

        wav_info info;
        if (!wav_info_read(path, &info))
            continue;

        tinyos_track *track = &audio->tracks[audio->count++];
        memset(track, 0, sizeof(*track));
        track_title(track->title, sizeof(track->title), path);
        snprintf(track->path, sizeof(track->path), "%s", path);
        track->duration_ms = wav_duration_ms(&info);
    }
    closedir(dir);
}

static int compare_tracks(const void *a, const void *b)
{
    const tinyos_track *left = a;
    const tinyos_track *right = b;
    return strcasecmp(left->title, right->title);
}

static void set_current_locked(tinyos_audio *audio)
{
    if (audio->count == 0) {
        audio->current = -1;
        audio->duration_ms = 0;
        audio->position_ms = 0;
        return;
    }
    if (audio->current < 0 || audio->current >= audio->count)
        audio->current = 0;
    audio->duration_ms = audio->tracks[audio->current].duration_ms;
}

static void stop_thread(tinyos_audio *audio, bool reset_position)
{
    pthread_mutex_lock(&audio->lock);
    bool running = audio->thread_running;
    audio->stop_requested = true;
    audio->playing = false;
    if (reset_position)
        audio->position_ms = 0;
    pthread_mutex_unlock(&audio->lock);

    if (running)
        pthread_join(audio->thread, NULL);

    pthread_mutex_lock(&audio->lock);
    audio->thread_running = false;
    if (reset_position)
        audio->position_ms = 0;
    pthread_mutex_unlock(&audio->lock);
}

static void *audio_thread(void *argument)
{
    tinyos_audio *audio = argument;
    char path[256];
    int start_ms;
    int duration_ms;
    int channels;
    int rate;
    uint32_t data_offset;
    uint32_t data_size;

    pthread_mutex_lock(&audio->lock);
    if (audio->current < 0 || audio->current >= audio->count) {
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return NULL;
    }
    snprintf(path, sizeof(path), "%s", audio->tracks[audio->current].path);
    start_ms = audio->position_ms;
    duration_ms = audio->duration_ms;
    pthread_mutex_unlock(&audio->lock);

    wav_info info;
    if (!wav_info_read(path, &info)) {
        TINYOS_LOGE("cannot parse %s", path);
        pthread_mutex_lock(&audio->lock);
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return NULL;
    }
    channels = info.channels;
    rate = (int)info.rate;
    data_offset = info.data_offset;
    data_size = info.data_size;

    FILE *file = fopen(path, "rb");
    if (!file) {
        TINYOS_LOGE("cannot open %s", path);
        pthread_mutex_lock(&audio->lock);
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return NULL;
    }

    snd_pcm_t *pcm = NULL;
    int error = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (error < 0) {
        TINYOS_LOGE("ALSA open failed: %s", snd_strerror(error));
        fclose(file);
        pthread_mutex_lock(&audio->lock);
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return NULL;
    }

    error = snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                               OUTPUT_CHANNELS, (unsigned int)rate, 1, 250000);
    if (error < 0) {
        TINYOS_LOGE("ALSA params failed: %s", snd_strerror(error));
        snd_pcm_close(pcm);
        fclose(file);
        pthread_mutex_lock(&audio->lock);
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return NULL;
    }

    uint64_t total_frames = data_size / ((uint64_t)channels * 2u);
    uint64_t skip_frames = (uint64_t)(start_ms > 0 ? start_ms : 0) * (uint64_t)rate / 1000u;
    if (skip_frames > total_frames)
        skip_frames = total_frames;
    if (fseek(file, (long)data_offset + (long)(skip_frames * (uint64_t)channels * 2u), SEEK_SET) != 0) {
        snd_pcm_close(pcm);
        fclose(file);
        pthread_mutex_lock(&audio->lock);
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return NULL;
    }

    int16_t input[PLAY_FRAMES * 2];
    int16_t output[PLAY_FRAMES * OUTPUT_CHANNELS];
    uint64_t played_frames = 0;

    while (true) {
        pthread_mutex_lock(&audio->lock);
        bool stop = audio->stop_requested;
        pthread_mutex_unlock(&audio->lock);
        if (stop)
            break;

        size_t frames = fread(input, (size_t)channels * 2u, PLAY_FRAMES, file);
        if (frames == 0)
            break;

        const int16_t *samples;
        if (channels == 1) {
            for (size_t i = 0; i < frames; i++) {
                output[i * 2] = input[i];
                output[i * 2 + 1] = input[i];
            }
            samples = output;
        } else {
            samples = input;
        }

        size_t offset = 0;
        while (offset < frames) {
            snd_pcm_sframes_t written = snd_pcm_writei(
                pcm, samples + offset * OUTPUT_CHANNELS, frames - offset);
            if (written < 0) {
                if (written == -EPIPE || written == -ESTRPIPE) {
                    if (snd_pcm_recover(pcm, (int)written, 1) < 0)
                        goto playback_done;
                    continue;
                }
                TINYOS_LOGW("ALSA write failed: %s", snd_strerror((int)written));
                goto playback_done;
            }
            offset += (size_t)written;
        }

        played_frames += frames;
        int position = start_ms + (int)(played_frames * 1000u / (uint64_t)rate);
        if (position > duration_ms)
            position = duration_ms;

        pthread_mutex_lock(&audio->lock);
        audio->position_ms = position;
        pthread_mutex_unlock(&audio->lock);
    }

playback_done:
    pthread_mutex_lock(&audio->lock);
    bool stopped = audio->stop_requested;
    pthread_mutex_unlock(&audio->lock);

    if (stopped)
        snd_pcm_drop(pcm);
    else
        snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(file);

    pthread_mutex_lock(&audio->lock);
    audio->playing = false;
    if (!stopped)
        audio->position_ms = 0;
    pthread_mutex_unlock(&audio->lock);
    return NULL;
}

static void start_thread(tinyos_audio *audio, int start_ms)
{
    stop_thread(audio, false);

    pthread_mutex_lock(&audio->lock);
    if (audio->count == 0) {
        audio->playing = false;
        pthread_mutex_unlock(&audio->lock);
        return;
    }
    set_current_locked(audio);
    int duration = audio->duration_ms;
    if (start_ms < 0)
        start_ms = 0;
    if (duration > 0 && start_ms > duration)
        start_ms = duration;
    audio->position_ms = start_ms;
    audio->stop_requested = false;
    audio->playing = true;
    audio->thread_running = true;
    pthread_mutex_unlock(&audio->lock);

    if (pthread_create(&audio->thread, NULL, audio_thread, audio) != 0) {
        TINYOS_LOGE("cannot create audio thread");
        pthread_mutex_lock(&audio->lock);
        audio->playing = false;
        audio->thread_running = false;
        pthread_mutex_unlock(&audio->lock);
    }
}

static void apply_volume(int volume)
{
    snd_mixer_t *mixer = NULL;
    if (snd_mixer_open(&mixer, 0) < 0)
        return;
    if (snd_mixer_attach(mixer, "default") < 0 ||
        snd_mixer_selem_register(mixer, NULL, NULL) < 0 ||
        snd_mixer_load(mixer) < 0) {
        snd_mixer_close(mixer);
        return;
    }

    for (snd_mixer_elem_t *element = snd_mixer_first_elem(mixer); element;
         element = snd_mixer_elem_next(element)) {
        if (!snd_mixer_selem_is_active(element) ||
            !snd_mixer_selem_has_playback_volume(element))
            continue;

        long minimum = 0;
        long maximum = 0;
        if (snd_mixer_selem_get_playback_volume_range(element, &minimum, &maximum) < 0)
            continue;
        long value = minimum + (maximum - minimum) * volume / 100;
        snd_mixer_selem_set_playback_volume_all(element, value);
    }
    snd_mixer_close(mixer);
}

tinyos_audio *tinyos_audio_create(void)
{
    tinyos_audio *audio = calloc(1, sizeof(*audio));
    if (!audio)
        return NULL;

    if (pthread_mutex_init(&audio->lock, NULL) != 0) {
        free(audio);
        return NULL;
    }
    audio->current = -1;
    audio->volume = 62;

    scan_directory(audio, TINYOS_AUDIO_SYSTEM_DIR);
    scan_directory(audio, TINYOS_AUDIO_USER_DIR);
    if (audio->count > 1)
        qsort(audio->tracks, (size_t)audio->count, sizeof(audio->tracks[0]), compare_tracks);

    pthread_mutex_lock(&audio->lock);
    set_current_locked(audio);
    pthread_mutex_unlock(&audio->lock);

    apply_volume(audio->volume);
    return audio;
}

void tinyos_audio_destroy(tinyos_audio *audio)
{
    if (!audio)
        return;
    stop_thread(audio, false);
    pthread_mutex_destroy(&audio->lock);
    free(audio);
}

void tinyos_audio_scan(tinyos_audio *audio)
{
    if (!audio)
        return;
    stop_thread(audio, true);

    pthread_mutex_lock(&audio->lock);
    audio->count = 0;
    audio->current = -1;
    pthread_mutex_unlock(&audio->lock);

    scan_directory(audio, TINYOS_AUDIO_SYSTEM_DIR);
    scan_directory(audio, TINYOS_AUDIO_USER_DIR);
    if (audio->count > 1)
        qsort(audio->tracks, (size_t)audio->count, sizeof(audio->tracks[0]), compare_tracks);

    pthread_mutex_lock(&audio->lock);
    audio->current = audio->count > 0 ? 0 : -1;
    set_current_locked(audio);
    audio->position_ms = 0;
    pthread_mutex_unlock(&audio->lock);
}

int tinyos_audio_count(const tinyos_audio *audio)
{
    return audio ? audio->count : 0;
}

const tinyos_track *tinyos_audio_track(const tinyos_audio *audio, int index)
{
    if (!audio || index < 0 || index >= audio->count)
        return NULL;
    return &audio->tracks[index];
}

int tinyos_audio_current(const tinyos_audio *audio)
{
    return audio ? audio->current : -1;
}

const tinyos_track *tinyos_audio_current_track(const tinyos_audio *audio)
{
    if (!audio || audio->current < 0 || audio->current >= audio->count)
        return NULL;
    return &audio->tracks[audio->current];
}

void tinyos_audio_play(tinyos_audio *audio, int index)
{
    if (!audio || audio->count == 0)
        return;

    stop_thread(audio, false);
    pthread_mutex_lock(&audio->lock);
    if (index < 0 || index >= audio->count)
        index = 0;
    audio->current = index;
    audio->position_ms = 0;
    set_current_locked(audio);
    pthread_mutex_unlock(&audio->lock);
    start_thread(audio, 0);
}

void tinyos_audio_toggle(tinyos_audio *audio)
{
    if (!audio || audio->count == 0)
        return;

    pthread_mutex_lock(&audio->lock);
    bool playing = audio->playing;
    int position = audio->position_ms;
    pthread_mutex_unlock(&audio->lock);

    if (playing)
        stop_thread(audio, false);
    else
        start_thread(audio, position);
}

void tinyos_audio_stop(tinyos_audio *audio)
{
    if (!audio)
        return;
    stop_thread(audio, true);
}

void tinyos_audio_next(tinyos_audio *audio, int direction)
{
    if (!audio || audio->count == 0)
        return;

    stop_thread(audio, false);
    pthread_mutex_lock(&audio->lock);
    audio->current = (audio->current + direction + audio->count) % audio->count;
    audio->position_ms = 0;
    set_current_locked(audio);
    pthread_mutex_unlock(&audio->lock);
    start_thread(audio, 0);
}

bool tinyos_audio_is_playing(const tinyos_audio *audio)
{
    if (!audio)
        return false;
    pthread_mutex_lock((pthread_mutex_t *)&audio->lock);
    bool playing = audio->playing;
    pthread_mutex_unlock((pthread_mutex_t *)&audio->lock);
    return playing;
}

int tinyos_audio_position_ms(const tinyos_audio *audio)
{
    if (!audio)
        return 0;
    pthread_mutex_lock((pthread_mutex_t *)&audio->lock);
    int position = audio->position_ms;
    pthread_mutex_unlock((pthread_mutex_t *)&audio->lock);
    return position;
}

int tinyos_audio_duration_ms(const tinyos_audio *audio)
{
    if (!audio)
        return 0;
    pthread_mutex_lock((pthread_mutex_t *)&audio->lock);
    int duration = audio->duration_ms;
    pthread_mutex_unlock((pthread_mutex_t *)&audio->lock);
    return duration;
}

int tinyos_audio_volume(const tinyos_audio *audio)
{
    if (!audio)
        return 0;
    pthread_mutex_lock((pthread_mutex_t *)&audio->lock);
    int volume = audio->volume;
    pthread_mutex_unlock((pthread_mutex_t *)&audio->lock);
    return volume;
}

void tinyos_audio_set_volume(tinyos_audio *audio, int volume)
{
    if (!audio)
        return;
    volume = tinyos_clamp_int(volume, 0, 100);
    pthread_mutex_lock(&audio->lock);
    audio->volume = volume;
    pthread_mutex_unlock(&audio->lock);
    apply_volume(volume);
}
