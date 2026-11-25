#include "../../platform.h"
#include "../../core/audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tinyos_audio {
    tinyos_track tracks[4];
    int count;
    int current;
    bool playing;
    int volume;
    int position_ms;
};

tinyos_audio *tinyos_audio_create(void)
{
    tinyos_audio *audio = calloc(1, sizeof(*audio));
    if (!audio)
        return NULL;

    audio->count = 1;
    audio->current = 0;
    audio->volume = 62;
    snprintf(audio->tracks[0].title, sizeof(audio->tracks[0].title), "TinyOS Theme");
    snprintf(audio->tracks[0].path, sizeof(audio->tracks[0].path),
             "/usr/share/tinyos/music/TinyOS Theme.wav");
    audio->tracks[0].duration_ms = 4000;
    return audio;
}

void tinyos_audio_destroy(tinyos_audio *audio)
{
    free(audio);
}

void tinyos_audio_scan(tinyos_audio *audio)
{
    (void)audio;
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
    return audio ? tinyos_audio_track(audio, audio->current) : NULL;
}

void tinyos_audio_play(tinyos_audio *audio, int index)
{
    if (!audio || audio->count == 0)
        return;
    if (index < 0 || index >= audio->count)
        index = 0;
    audio->current = index;
    audio->position_ms = 0;
    audio->playing = true;
}

void tinyos_audio_toggle(tinyos_audio *audio)
{
    if (!audio || audio->count == 0)
        return;
    audio->playing = !audio->playing;
}

void tinyos_audio_stop(tinyos_audio *audio)
{
    if (!audio)
        return;
    audio->playing = false;
    audio->position_ms = 0;
}

void tinyos_audio_next(tinyos_audio *audio, int direction)
{
    if (!audio || audio->count == 0)
        return;
    audio->current = (audio->current + direction + audio->count) % audio->count;
    audio->position_ms = 0;
    audio->playing = true;
}

bool tinyos_audio_is_playing(const tinyos_audio *audio)
{
    return audio ? audio->playing : false;
}

int tinyos_audio_position_ms(const tinyos_audio *audio)
{
    return audio ? audio->position_ms : 0;
}

int tinyos_audio_duration_ms(const tinyos_audio *audio)
{
    const tinyos_track *track = tinyos_audio_current_track(audio);
    return track ? track->duration_ms : 0;
}

int tinyos_audio_volume(const tinyos_audio *audio)
{
    return audio ? audio->volume : 0;
}

void tinyos_audio_set_volume(tinyos_audio *audio, int volume)
{
    if (!audio)
        return;
    if (volume < 0)
        volume = 0;
    if (volume > 100)
        volume = 100;
    audio->volume = volume;
}
