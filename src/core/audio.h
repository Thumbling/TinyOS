#ifndef TINYOS_CORE_AUDIO_H
#define TINYOS_CORE_AUDIO_H

#include <stdbool.h>

#define TINYOS_AUDIO_SYSTEM_DIR "/usr/share/tinyos/music"
#define TINYOS_AUDIO_USER_DIR "/data/music"

typedef struct {
    char title[96];
    char path[256];
    int duration_ms;
} tinyos_track;

typedef struct tinyos_audio tinyos_audio;

tinyos_audio *tinyos_audio_create(void);
void tinyos_audio_destroy(tinyos_audio *audio);

void tinyos_audio_scan(tinyos_audio *audio);
int tinyos_audio_count(const tinyos_audio *audio);
const tinyos_track *tinyos_audio_track(const tinyos_audio *audio, int index);
int tinyos_audio_current(const tinyos_audio *audio);
const tinyos_track *tinyos_audio_current_track(const tinyos_audio *audio);

void tinyos_audio_play(tinyos_audio *audio, int index);
void tinyos_audio_toggle(tinyos_audio *audio);
void tinyos_audio_stop(tinyos_audio *audio);
void tinyos_audio_next(tinyos_audio *audio, int direction);

bool tinyos_audio_is_playing(const tinyos_audio *audio);
int tinyos_audio_position_ms(const tinyos_audio *audio);
int tinyos_audio_duration_ms(const tinyos_audio *audio);
int tinyos_audio_volume(const tinyos_audio *audio);
void tinyos_audio_set_volume(tinyos_audio *audio, int volume);

#endif
