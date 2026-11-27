/* Minimal ALSA ABI surface used only by the ARM link audit.
 *
 * This is deliberately not a replacement for alsa-lib at runtime. It lets the
 * real audio_alsa.c translation unit participate in an ARM link when the
 * Buildroot sysroot is unavailable. The matching stub implementation is
 * tests/arm-link/alsa_stub.c. */
#ifndef TINYOS_ARM_LINK_ASOUNDLIB_H
#define TINYOS_ARM_LINK_ASOUNDLIB_H

#include <errno.h>
#include <stddef.h>

typedef struct snd_pcm snd_pcm_t;
typedef long snd_pcm_sframes_t;
typedef unsigned long snd_pcm_uframes_t;
typedef int snd_pcm_format_t;
typedef int snd_pcm_access_t;
typedef struct snd_mixer snd_mixer_t;
typedef struct snd_mixer_elem snd_mixer_elem_t;
typedef struct snd_mixer_selem_regopt snd_mixer_selem_regopt;

enum {
    SND_PCM_STREAM_PLAYBACK = 0,
    SND_PCM_FORMAT_S16_LE = 2,
    SND_PCM_ACCESS_RW_INTERLEAVED = 3,
};

int snd_pcm_open(snd_pcm_t **pcm, const char *name, int stream, int mode);
int snd_pcm_set_params(snd_pcm_t *pcm, snd_pcm_format_t format,
                       snd_pcm_access_t access, unsigned channels,
                       unsigned rate, int soft_resample, unsigned latency);
snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t *pcm, const void *buffer,
                                 snd_pcm_uframes_t size);
int snd_pcm_recover(snd_pcm_t *pcm, int err, int silent);
int snd_pcm_drop(snd_pcm_t *pcm);
int snd_pcm_drain(snd_pcm_t *pcm);
int snd_pcm_close(snd_pcm_t *pcm);
const char *snd_strerror(int err);

int snd_mixer_open(snd_mixer_t **mixer, int mode);
int snd_mixer_attach(snd_mixer_t *mixer, const char *name);
int snd_mixer_selem_register(snd_mixer_t *mixer,
                             snd_mixer_selem_regopt *options, void *classp);
int snd_mixer_load(snd_mixer_t *mixer);
int snd_mixer_close(snd_mixer_t *mixer);
snd_mixer_elem_t *snd_mixer_first_elem(snd_mixer_t *mixer);
snd_mixer_elem_t *snd_mixer_elem_next(snd_mixer_elem_t *element);
int snd_mixer_selem_is_active(snd_mixer_elem_t *element);
int snd_mixer_selem_has_playback_volume(snd_mixer_elem_t *element);
int snd_mixer_selem_get_playback_volume_range(snd_mixer_elem_t *element,
                                              long *minimum, long *maximum);
int snd_mixer_selem_set_playback_volume_all(snd_mixer_elem_t *element,
                                            long value);

#endif
