#include "ui/fonts.h"

#include <stddef.h>
#include <string.h>

#define FONT_DECL(family, size) extern const lv_font_t tinyos_##family##_##size;
#define FONT_ROW(family) { \
    &tinyos_##family##_8, &tinyos_##family##_10, &tinyos_##family##_12, \
    &tinyos_##family##_14, &tinyos_##family##_16, &tinyos_##family##_20, \
    &tinyos_##family##_24, &tinyos_##family##_28 \
}

FONT_DECL(inter, 8) FONT_DECL(inter, 10) FONT_DECL(inter, 12) FONT_DECL(inter, 14)
FONT_DECL(inter, 16) FONT_DECL(inter, 20) FONT_DECL(inter, 24) FONT_DECL(inter, 28)
FONT_DECL(plex, 8) FONT_DECL(plex, 10) FONT_DECL(plex, 12) FONT_DECL(plex, 14)
FONT_DECL(plex, 16) FONT_DECL(plex, 20) FONT_DECL(plex, 24) FONT_DECL(plex, 28)
FONT_DECL(spacegrotesk, 8) FONT_DECL(spacegrotesk, 10) FONT_DECL(spacegrotesk, 12) FONT_DECL(spacegrotesk, 14)
FONT_DECL(spacegrotesk, 16) FONT_DECL(spacegrotesk, 20) FONT_DECL(spacegrotesk, 24) FONT_DECL(spacegrotesk, 28)
FONT_DECL(jetbrainsmono, 8) FONT_DECL(jetbrainsmono, 10) FONT_DECL(jetbrainsmono, 12) FONT_DECL(jetbrainsmono, 14)
FONT_DECL(jetbrainsmono, 16) FONT_DECL(jetbrainsmono, 20) FONT_DECL(jetbrainsmono, 24) FONT_DECL(jetbrainsmono, 28)
FONT_DECL(atkinson, 8) FONT_DECL(atkinson, 10) FONT_DECL(atkinson, 12) FONT_DECL(atkinson, 14)
FONT_DECL(atkinson, 16) FONT_DECL(atkinson, 20) FONT_DECL(atkinson, 24) FONT_DECL(atkinson, 28)

static const int sizes[TINYOS_FONT_SIZE_COUNT] = {8, 10, 12, 14, 16, 20, 24, 28};

static const lv_font_t *const montserrat[TINYOS_FONT_SIZE_COUNT] = {
    &lv_font_montserrat_8, &lv_font_montserrat_10, &lv_font_montserrat_12,
    &lv_font_montserrat_14, &lv_font_montserrat_16, &lv_font_montserrat_20,
    &lv_font_montserrat_24, &lv_font_montserrat_28,
};

static const lv_font_t *const inter[TINYOS_FONT_SIZE_COUNT] = FONT_ROW(inter);
static const lv_font_t *const plex[TINYOS_FONT_SIZE_COUNT] = FONT_ROW(plex);
static const lv_font_t *const spacegrotesk[TINYOS_FONT_SIZE_COUNT] = FONT_ROW(spacegrotesk);
static const lv_font_t *const jetbrainsmono[TINYOS_FONT_SIZE_COUNT] = FONT_ROW(jetbrainsmono);
static const lv_font_t *const atkinson[TINYOS_FONT_SIZE_COUNT] = FONT_ROW(atkinson);

static const lv_font_t *const *const families[TINYOS_FONT_FAMILY_COUNT] = {
    montserrat, inter, plex, spacegrotesk, jetbrainsmono, atkinson,
};

static const char *const family_names[TINYOS_FONT_FAMILY_COUNT] = {
    "Montserrat", "Inter", "IBM Plex", "Space Grotesk", "JetBrains", "Atkinson HL",
};

static int active_family;

static int find_size(int size)
{
    int best = 0;
    int distance = 1000;
    for (int i = 0; i < TINYOS_FONT_SIZE_COUNT; i++) {
        int d = size > sizes[i] ? size - sizes[i] : sizes[i] - size;
        if (d < distance) {
            distance = d;
            best = i;
        }
    }
    return best;
}

static int legacy_size(const lv_font_t *font)
{
    const lv_font_t *const legacy[TINYOS_FONT_SIZE_COUNT] = {
        &lv_font_montserrat_8, &lv_font_montserrat_10, &lv_font_montserrat_12,
        &lv_font_montserrat_14, &lv_font_montserrat_16, &lv_font_montserrat_20,
        &lv_font_montserrat_24, &lv_font_montserrat_28,
    };
    for (int i = 0; i < TINYOS_FONT_SIZE_COUNT; i++) {
        if (font == legacy[i]) {
            return sizes[i];
        }
    }
    return 0;
}

void tinyos_fonts_select(int family)
{
    if (family < 0 || family >= TINYOS_FONT_FAMILY_COUNT) {
        family = 0;
    }
    active_family = family;
}

int tinyos_fonts_active(void)
{
    return active_family;
}

const char *tinyos_font_family_name(int family)
{
    if (family < 0 || family >= TINYOS_FONT_FAMILY_COUNT) {
        family = active_family;
    }
    return family_names[family];
}

const char *tinyos_font_family_short_name(int family)
{
    const char *name = tinyos_font_family_name(family);
    return strcmp(name, "Space Grotesk") == 0 ? "Grotesk" : name;
}

const lv_font_t *tinyos_font_ui(int size)
{
    return families[active_family][find_size(size)];
}

const lv_font_t *tinyos_font_family(int family, int size)
{
    if (family < 0 || family >= TINYOS_FONT_FAMILY_COUNT) {
        family = active_family;
    }
    return families[family][find_size(size)];
}

const lv_font_t *tinyos_font_map(const lv_font_t *requested)
{
    int size = legacy_size(requested);
    if (size == 0) {
        return requested;
    }
    return tinyos_font_ui(size);
}
