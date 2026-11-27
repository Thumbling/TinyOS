/* Link-audit implementation of the ALSA calls used by audio_alsa.c.
 * It never enters the firmware image; the ARM audit links it only to prove
 * that the real platform glue has no unresolved symbols of its own. */
#include <alsa/asoundlib.h>

int snd_pcm_open(snd_pcm_t **pcm, const char *name, int stream, int mode)
{
    (void)name; (void)stream; (void)mode;
    *pcm = 0;
    return -1;
}

int snd_pcm_set_params(snd_pcm_t *pcm, snd_pcm_format_t format,
                       snd_pcm_access_t access, unsigned channels,
                       unsigned rate, int soft_resample, unsigned latency)
{
    (void)pcm; (void)format; (void)access; (void)channels; (void)rate;
    (void)soft_resample; (void)latency;
    return -1;
}

snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t *pcm, const void *buffer,
                                 snd_pcm_uframes_t size)
{
    (void)pcm; (void)buffer; (void)size;
    return -1;
}

int snd_pcm_recover(snd_pcm_t *pcm, int err, int silent)
{
    (void)pcm; (void)err; (void)silent;
    return -1;
}

int snd_pcm_drop(snd_pcm_t *pcm) { (void)pcm; return 0; }
int snd_pcm_drain(snd_pcm_t *pcm) { (void)pcm; return 0; }
int snd_pcm_close(snd_pcm_t *pcm) { (void)pcm; return 0; }

const char *snd_strerror(int err)
{
    (void)err;
    return "link-audit ALSA stub";
}

int snd_mixer_open(snd_mixer_t **mixer, int mode)
{
    (void)mode;
    *mixer = 0;
    return -1;
}

int snd_mixer_attach(snd_mixer_t *mixer, const char *name)
{
    (void)mixer; (void)name;
    return -1;
}

int snd_mixer_selem_register(snd_mixer_t *mixer,
                             snd_mixer_selem_regopt *options, void *classp)
{
    (void)mixer; (void)options; (void)classp;
    return -1;
}

int snd_mixer_load(snd_mixer_t *mixer) { (void)mixer; return -1; }
int snd_mixer_close(snd_mixer_t *mixer) { (void)mixer; return 0; }

snd_mixer_elem_t *snd_mixer_first_elem(snd_mixer_t *mixer)
{
    (void)mixer;
    return 0;
}

snd_mixer_elem_t *snd_mixer_elem_next(snd_mixer_elem_t *element)
{
    (void)element;
    return 0;
}

int snd_mixer_selem_is_active(snd_mixer_elem_t *element)
{
    (void)element;
    return 0;
}

int snd_mixer_selem_has_playback_volume(snd_mixer_elem_t *element)
{
    (void)element;
    return 0;
}

int snd_mixer_selem_get_playback_volume_range(snd_mixer_elem_t *element,
                                              long *minimum, long *maximum)
{
    (void)element;
    if (minimum)
        *minimum = 0;
    if (maximum)
        *maximum = 0;
    return -1;
}

int snd_mixer_selem_set_playback_volume_all(snd_mixer_elem_t *element,
                                            long value)
{
    (void)element; (void)value;
    return -1;
}
