#ifndef TINYOS_MODULES_MUSIC_H
#define TINYOS_MODULES_MUSIC_H

enum {
    TINYOS_MUSIC_AUTO = -1,
    TINYOS_MUSIC_NORMAL = 0,
    TINYOS_MUSIC_EMPTY = 1,
    TINYOS_MUSIC_LOADING = 2,
    TINYOS_MUSIC_ERROR = 3,
};

/* Simulator/test hook: -1 follows the real data, the other values force a
 * state so empty/loading/error can be rendered and inspected. */
void tinyos_music_force_state(int state);

#endif
