/* Deterministic headless self-test for the TinyOS mGBA game path.
 *
 * The test ROMs are generated into build/emulator-selftest/roms by
 * tools/gen_emulator_test_roms.py. They contain no commercial game code and
 * no BIOS. The same session API is used by the firmware game loop.
 */
#include "emulator/session.h"
#include "emulator/game_keys.h"
#include "emulator/input_map.h"
#include "platform/linux/fbdev_blit.h"

#include <mgba/internal/gb/input.h>
#include <mgba/internal/gba/input.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PANEL_SIZE 240u
#define PANEL_PIXELS (PANEL_SIZE * PANEL_SIZE)
#define SAVE_PATTERN_GB { 0x5A, 0xA5, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 }

static int failures;
static int checks;

static void check(bool condition, const char *what)
{
    checks++;
    if (condition)
        return;
    failures++;
    printf("emulator selftest: FAIL: %s\n", what);
}

static void check_u32(const char *what, uint32_t actual, uint32_t expected)
{
    checks++;
    if (actual == expected)
        return;
    failures++;
    printf("emulator selftest: FAIL: %s: got 0x%08X, expected 0x%08X\n",
           what, actual, expected);
}

static void check_u16(const char *what, uint16_t actual, uint16_t expected)
{
    checks++;
    if (actual == expected)
        return;
    failures++;
    printf("emulator selftest: FAIL: %s: got 0x%04X, expected 0x%04X\n",
           what, actual, expected);
}

static void check_geometry(const char *what, unsigned actual, unsigned expected)
{
    checks++;
    if (actual == expected)
        return;
    failures++;
    printf("emulator selftest: FAIL: %s: got %u, expected %u\n",
           what, actual, expected);
}

typedef struct {
    const char *name;
    const char *file;
    tinyos_rom_system system;
    unsigned core_width;
    unsigned core_height;
    unsigned bars;
    unsigned image_height;
    size_t save_size;
} system_spec;

static const system_spec systems[] = {
    { "GB",  "gb/tinyos-selftest.gb",  TINYOS_ROM_GB,  160, 144, 12, 216, 8192 },
    { "GBC", "gbc/tinyos-selftest.gbc", TINYOS_ROM_GBC, 160, 144, 12, 216, 8192 },
    { "GBA", "gba/tinyos-selftest.gba", TINYOS_ROM_GBA, 240, 160, 40, 160, 32768 },
};

static void make_entry(tinyos_rom_entry *entry, const char *root,
                       const system_spec *spec)
{
    memset(entry, 0, sizeof(*entry));
    snprintf(entry->path, sizeof(entry->path), "%s/%s", root, spec->file);
    snprintf(entry->name, sizeof(entry->name), "%s selftest", spec->name);
    entry->system = spec->system;
}

static uint16_t panel_at(const uint16_t *panel, unsigned x, unsigned y)
{
    return panel[y * PANEL_SIZE + x];
}

static void check_borders(const system_spec *spec, const uint16_t *panel)
{
    char label[96];
    for (unsigned y = 0; y < spec->bars; y++) {
        for (unsigned x = 0; x < PANEL_SIZE; x++) {
            if (panel_at(panel, x, y) != 0) {
                snprintf(label, sizeof(label),
                         "%s top letterbox pixel (%u,%u)", spec->name, x, y);
                check(false, label);
                return;
            }
        }
    }
    for (unsigned y = spec->bars + spec->image_height; y < PANEL_SIZE; y++) {
        for (unsigned x = 0; x < PANEL_SIZE; x++) {
            if (panel_at(panel, x, y) != 0) {
                snprintf(label, sizeof(label),
                         "%s bottom letterbox pixel (%u,%u)", spec->name, x, y);
                check(false, label);
                return;
            }
        }
    }
    check(true, "letterbox rows are black");
}

static void check_gb_panel(const system_spec *spec, const uint16_t *panel,
                           bool color)
{
    static const uint16_t dmg_palette[4] = { 0xFFFF, 0xAD75, 0x528A, 0x0000 };
    static const uint16_t cgb_palette[4] = { 0xF800, 0x07E0, 0x001F, 0xFFFF };
    const uint16_t *palette = color ? cgb_palette : dmg_palette;
    char label[128];

    for (unsigned y = 0; y < spec->image_height; y++) {
        unsigned source_y = (y * 2u) / 3u;
        unsigned band = (source_y % 8u) / 2u;
        uint16_t expected = palette[band];
        unsigned panel_y = spec->bars + y;
        if (panel_at(panel, 0, panel_y) != expected ||
            panel_at(panel, 1, panel_y) != expected ||
            panel_at(panel, 80, panel_y) != expected ||
            panel_at(panel, 159, panel_y) != expected ||
            panel_at(panel, 239, panel_y) != expected) {
            snprintf(label, sizeof(label),
                     "%s scaled row %u (source band %u)", spec->name, y, band);
            check(false, label);
            return;
        }
    }

    /* Concrete corners, not only a non-zero or uniform-image check. */
    check_u16("GB/GBC first active pixel", panel_at(panel, 0, spec->bars),
              palette[0]);
    check_u16("GB/GBC first band pixel", panel_at(panel, 0, spec->bars + 3),
              palette[1]);
    check_u16("GB/GBC second band pixel", panel_at(panel, 0, spec->bars + 6),
              palette[2]);
    check_u16("GB/GBC third band pixel", panel_at(panel, 0, spec->bars + 9),
              palette[3]);
    check_u16("GB/GBC right edge", panel_at(panel, 239, spec->bars + 3),
              palette[1]);
    check(true, "GB/GBC 3:2 nearest-neighbour image");
}

static void check_gba_panel(const system_spec *spec, const uint16_t *panel)
{
    /* GBA palette entries are BGR555. mGBA converts the four test entries
     * 0x001F/0x03E0/0x7C00/0x7FFF to panel RGB565 as follows. */
    static const uint16_t first_row[8] = {
        0xF800, 0xF800, 0x07E0, 0x07E0,
        0x001F, 0x001F, 0xFFFF, 0xFFFF,
    };
    char label[128];

    for (unsigned x = 0; x < 8; x++) {
        if (panel_at(panel, x, spec->bars) != first_row[x]) {
            snprintf(label, sizeof(label), "GBA pixel x=%u", x);
            check_u16(label, panel_at(panel, x, spec->bars), first_row[x]);
            return;
        }
    }
    for (unsigned x = 8; x < PANEL_SIZE; x++) {
        if (panel_at(panel, x, spec->bars) != 0) {
            snprintf(label, sizeof(label), "GBA untouched pixel x=%u", x);
            check(false, label);
            return;
        }
    }
    for (unsigned y = 1; y < spec->image_height; y++) {
        if (panel_at(panel, 0, spec->bars + y) != 0 ||
            panel_at(panel, 239, spec->bars + y) != 0) {
            snprintf(label, sizeof(label), "GBA untouched active row %u", y);
            check(false, label);
            return;
        }
    }
    check(true, "GBA 240x160 native image");
}

static void run_geometry_test(const char *root, const system_spec *spec)
{
    tinyos_rom_entry entry;
    make_entry(&entry, root, spec);

    tinyos_emulator_session *session = tinyos_emulator_session_open(&entry);
    check(session != NULL, "session opens");
    if (!session) {
        printf("emulator selftest: %s open error: %s\n", spec->name,
               tinyos_emulator_session_last_error());
        return;
    }

    for (int frame = 0; frame < 60; frame++)
        check(tinyos_emulator_session_run_frame(session, 0), "frame runs");

    const tinyos_emulator_geometry *geometry =
        tinyos_emulator_session_geometry(session);
    check(geometry != NULL, "geometry available");
    if (geometry) {
        check_geometry("core width", geometry->core_width, spec->core_width);
        check_geometry("core height", geometry->core_height, spec->core_height);
        check_geometry("panel width", geometry->panel_width, PANEL_SIZE);
        check_geometry("panel height", geometry->panel_height, PANEL_SIZE);
        check_geometry("image x", geometry->image_x, 0);
        check_geometry("image y", geometry->image_y, spec->bars);
        check_geometry("image width", geometry->image_width, PANEL_SIZE);
        check_geometry("image height", geometry->image_height,
                       spec->image_height);
    }

    const uint16_t *panel = tinyos_emulator_session_panel(session);
    check(panel != NULL, "panel available");
    if (panel) {
        check_borders(spec, panel);
        if (spec->system == TINYOS_ROM_GBA)
            check_gba_panel(spec, panel);
        else
            check_gb_panel(spec, panel, spec->system == TINYOS_ROM_GBC);
    }

    tinyos_emulator_session_close(session, false);
}

static void run_fbdev_test(const char *root)
{
    tinyos_rom_entry entry;
    const system_spec *spec = &systems[1]; /* GBC gives four distinct colours. */
    make_entry(&entry, root, spec);
    tinyos_emulator_session *session = tinyos_emulator_session_open(&entry);
    check(session != NULL, "framebuffer session opens");
    if (!session)
        return;
    for (int frame = 0; frame < 60; frame++)
        tinyos_emulator_session_run_frame(session, 0);
    const uint16_t *panel = tinyos_emulator_session_panel(session);
    check(panel != NULL, "framebuffer panel available");
    if (panel) {
        static uint8_t rgb565[PANEL_PIXELS * 2];
        tinyos_fbdev_geometry fb16 = {
            .memory = rgb565,
            .size = sizeof(rgb565),
            .stride = PANEL_SIZE * 2,
            .width = PANEL_SIZE,
            .height = PANEL_SIZE,
            .xoffset = 0,
            .yoffset = 0,
            .bits_per_pixel = 16,
        };
        tinyos_fbdev_blit_rgb565(&fb16, 0, 0, PANEL_SIZE - 1, PANEL_SIZE - 1,
                                 (const uint8_t *)panel);
        bool byte_order_ok = true;
        for (unsigned i = 0; i < PANEL_PIXELS; i++) {
            if (rgb565[i * 2] != (uint8_t)(panel[i] & 0xFF) ||
                rgb565[i * 2 + 1] != (uint8_t)(panel[i] >> 8)) {
                byte_order_ok = false;
                break;
            }
        }
        check(byte_order_ok, "16-bpp RGB565 byte order over 240x240");

        /* The 32-bpp buffer is not initialized by the 16-bpp blit. */
        static uint8_t rgb888[PANEL_PIXELS * 4];
        tinyos_fbdev_geometry fb32 = {
            .memory = rgb888,
            .size = sizeof(rgb888),
            .stride = PANEL_SIZE * 4,
            .width = PANEL_SIZE,
            .height = PANEL_SIZE,
            .xoffset = 0,
            .yoffset = 0,
            .bits_per_pixel = 32,
        };
        memset(rgb888, 0xA5, sizeof(rgb888));
        tinyos_fbdev_blit_rgb565(&fb32, 0, 0, PANEL_SIZE - 1, PANEL_SIZE - 1,
                                 (const uint8_t *)panel);
        bool ok = true;
        for (unsigned i = 0; i < PANEL_PIXELS; i++) {
            uint16_t pixel = panel[i];
            unsigned r = (pixel >> 11) & 0x1F;
            unsigned g = (pixel >> 5) & 0x3F;
            unsigned b = pixel & 0x1F;
            uint8_t r8 = (uint8_t)((r * 255 + 15) / 31);
            uint8_t g8 = (uint8_t)((g * 255 + 31) / 63);
            uint8_t b8 = (uint8_t)((b * 255 + 15) / 31);
            if (rgb888[i * 4] != b8 || rgb888[i * 4 + 1] != g8 ||
                rgb888[i * 4 + 2] != r8 || rgb888[i * 4 + 3] != 0xFF) {
                ok = false;
                break;
            }
        }
        check(ok, "32-bpp panel path over 240x240");
    }
    tinyos_emulator_session_close(session, false);
}

static void remove_save(const char *path)
{
    if (path && path[0])
        remove(path);
}

static void run_save_test(const char *root, const system_spec *spec)
{
    tinyos_rom_entry entry;
    make_entry(&entry, root, spec);

    tinyos_emulator_session *first = tinyos_emulator_session_open(&entry);
    check(first != NULL, "save session opens");
    if (!first)
        return;
    const char *save_path = tinyos_emulator_session_save_path(first);
    check(save_path != NULL && save_path[0] != '\0', "save path available");
    char save_path_copy[TINYOS_ROM_PATH_MAX];
    snprintf(save_path_copy, sizeof(save_path_copy), "%s",
             save_path ? save_path : "");
    tinyos_emulator_session_close(first, false);
    remove_save(save_path_copy);
    /* Reopen after removing the file so the first run exercises a truly
     * first-run cartridge with no pre-existing .sav. */
    first = tinyos_emulator_session_open(&entry);
    check(first != NULL, "fresh first run opens");
    if (!first)
        return;

    for (int frame = 0; frame < 30; frame++)
        tinyos_emulator_session_run_frame(first, 0);

    void *first_save = NULL;
    size_t first_size = tinyos_emulator_session_clone_save(first, &first_save);
    check(first_size == spec->save_size, "first save size");
    check(first_save != NULL, "first save clone exists");
    if (first_save && first_size >= 8) {
        const uint8_t *bytes = first_save;
        if (spec->system == TINYOS_ROM_GBA) {
            const uint8_t expected[8] = { 0x44, 0x44, 0x44, 0x44,
                                          0x88, 0x88, 0x88, 0x88 };
            check(memcmp(bytes, expected, sizeof(expected)) == 0,
                  "GBA SRAM byte-bus pattern");
        } else {
            const uint8_t expected[8] = SAVE_PATTERN_GB;
            check(memcmp(bytes, expected, sizeof(expected)) == 0,
                  "GB/GBC SRAM save pattern");
        }
    }

    tinyos_emulator_session_close(first, true);

    struct stat info;
    check(save_path_copy[0] && stat(save_path_copy, &info) == 0 &&
              (size_t)info.st_size == first_size,
          "save file written with clone size");

    uint8_t *file_bytes = NULL;
    if (save_path_copy[0] && first_size && first_size <= 128 * 1024) {
        file_bytes = malloc(first_size);
        FILE *file = save_path_copy[0] ? fopen(save_path_copy, "rb") : NULL;
        bool read_ok = false;
        if (file && file_bytes) {
            read_ok = fread(file_bytes, 1, first_size, file) == first_size;
            fclose(file);
        }
        check(read_ok, "save file reads back");
        if (read_ok && first_save)
            check(memcmp(file_bytes, first_save, first_size) == 0,
                  "save file equals in-memory save");
    }

    printf("emulator selftest: %s reopening saved cartridge\n", spec->name);
    fflush(stdout);
    tinyos_emulator_session *second = tinyos_emulator_session_open(&entry);
    check(second != NULL, "second run opens with save");
    if (second) {
        void *second_save = NULL;
        size_t second_size =
            tinyos_emulator_session_clone_save(second, &second_save);
        check(second_size == first_size, "roundtrip save size");
        check(second_save != NULL, "roundtrip save clone exists");
        if (first_save && second_save && first_size == second_size)
            check(memcmp(first_save, second_save, first_size) == 0,
                  "save roundtrip over two runs");
        tinyos_emulator_session_close(second, false);
    }

    free(first_save);
    free(file_bytes);
}

static void run_input_test(void)
{
    struct physical_key {
        uint16_t code;
        uint32_t key;
    };
    static const struct physical_key keys[] = {
        { TINYOS_EV_BTN_SOUTH, TINYOS_GAME_KEY_B },
        { TINYOS_EV_BTN_EAST, TINYOS_GAME_KEY_A },
        { TINYOS_EV_BTN_NORTH, TINYOS_GAME_KEY_X },
        { TINYOS_EV_BTN_WEST, TINYOS_GAME_KEY_Y },
        { TINYOS_EV_BTN_TL, TINYOS_GAME_KEY_L },
        { TINYOS_EV_BTN_TR, TINYOS_GAME_KEY_R },
        { TINYOS_EV_BTN_DPAD_UP, TINYOS_GAME_KEY_UP },
        { TINYOS_EV_BTN_DPAD_DOWN, TINYOS_GAME_KEY_DOWN },
        { TINYOS_EV_BTN_DPAD_LEFT, TINYOS_GAME_KEY_LEFT },
        { TINYOS_EV_BTN_DPAD_RIGHT, TINYOS_GAME_KEY_RIGHT },
        { TINYOS_EV_BTN_START, TINYOS_GAME_KEY_START },
        { TINYOS_EV_BTN_SELECT, TINYOS_GAME_KEY_SELECT },
        { TINYOS_EV_KEY_POWER, TINYOS_GAME_KEY_EXIT },
    };

    uint32_t ten_device_bits = 0;
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        uint32_t mapped = tinyos_game_key_from_evdev(keys[i].code);
        check_u32("evdev mapping", mapped, keys[i].key);
        if (i < 10)
            ten_device_bits |= mapped;
    }
    check_u32("ten direct device buttons map to ten bits", ten_device_bits,
              TINYOS_GAME_KEY_A | TINYOS_GAME_KEY_B | TINYOS_GAME_KEY_X |
                  TINYOS_GAME_KEY_Y | TINYOS_GAME_KEY_L | TINYOS_GAME_KEY_R |
                  TINYOS_GAME_KEY_UP | TINYOS_GAME_KEY_DOWN |
                  TINYOS_GAME_KEY_LEFT | TINYOS_GAME_KEY_RIGHT);
    check_u32("unknown evdev key ignored", tinyos_game_key_from_evdev(0x2FF), 0);

    const uint32_t gba_expected =
        (1u << GBA_KEY_A) | (1u << GBA_KEY_B) | (1u << GBA_KEY_SELECT) |
        (1u << GBA_KEY_START) | (1u << GBA_KEY_RIGHT) | (1u << GBA_KEY_LEFT) |
        (1u << GBA_KEY_UP) | (1u << GBA_KEY_DOWN) | (1u << GBA_KEY_R) |
        (1u << GBA_KEY_L);
    const uint32_t gb_expected =
        (1u << GB_KEY_A) | (1u << GB_KEY_B) | (1u << GB_KEY_SELECT) |
        (1u << GB_KEY_START) | (1u << GB_KEY_RIGHT) | (1u << GB_KEY_LEFT) |
        (1u << GB_KEY_UP) | (1u << GB_KEY_DOWN);

    uint32_t all_core = TINYOS_GAME_KEY_A | TINYOS_GAME_KEY_B |
                        TINYOS_GAME_KEY_SELECT | TINYOS_GAME_KEY_START |
                        TINYOS_GAME_KEY_RIGHT | TINYOS_GAME_KEY_LEFT |
                        TINYOS_GAME_KEY_UP | TINYOS_GAME_KEY_DOWN |
                        TINYOS_GAME_KEY_R | TINYOS_GAME_KEY_L;
    check_u32("GBA ten-button core mask",
              tinyos_emulator_controller_keys(all_core, TINYOS_ROM_GBA),
              gba_expected);
    check_u32("GB eight-button core mask",
              tinyos_emulator_controller_keys(all_core, TINYOS_ROM_GB),
              gb_expected);
    check_u32("X/Y are not core Game Boy buttons",
              tinyos_emulator_controller_keys(
                  TINYOS_GAME_KEY_X | TINYOS_GAME_KEY_Y, TINYOS_ROM_GB), 0);
    check_u32("X/Y are not core GBA buttons",
              tinyos_emulator_controller_keys(
                  TINYOS_GAME_KEY_X | TINYOS_GAME_KEY_Y, TINYOS_ROM_GBA), 0);
}

static void run_gesture_test(void)
{
    tinyos_game_exit_gesture gesture;
    const uint32_t chord = TINYOS_GAME_KEY_X | TINYOS_GAME_KEY_Y;

    tinyos_game_exit_gesture_init(&gesture);
    check(!tinyos_game_exit_gesture_update(&gesture, 0, 0),
          "gesture idle does not fire");
    check(!tinyos_game_exit_gesture_update(&gesture, TINYOS_GAME_KEY_X, 10),
          "X alone does not fire");
    check(!tinyos_game_exit_gesture_update(&gesture, TINYOS_GAME_KEY_Y, 20),
          "Y alone does not fire");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 30),
          "chord starts but does not fire immediately");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 929),
          "chord under 900 ms does not fire");
    check(tinyos_game_exit_gesture_update(&gesture, chord, 930),
          "chord at exactly 900 ms fires once");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 5000),
          "fired gesture does not fire twice");
    check(!tinyos_game_exit_gesture_update(&gesture, 0, 5010),
          "released fired gesture stays quiet");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 6000),
          "re-held fired gesture stays latched");

    tinyos_game_exit_gesture_init(&gesture);
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 1000),
          "second gesture starts");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 1899),
          "second gesture under 900 ms does not fire");
    check(!tinyos_game_exit_gesture_update(&gesture, 0, 1900),
          "release before threshold cancels");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 2000),
          "new hold after cancellation starts");
    check(tinyos_game_exit_gesture_update(&gesture, chord, 2900),
          "new hold fires at 900 ms");
    check(!tinyos_game_exit_gesture_update(&gesture, chord, 3900),
          "new hold fires only once");
}

static void run_pacing_test(void)
{
    unsigned phase = 0;
    unsigned sixteen = 0;
    unsigned seventeen = 0;
    uint64_t total = 0;
    for (unsigned frame = 0; frame < TINYOS_GAME_PACING_WINDOW; frame++) {
        uint32_t delay = tinyos_game_pacing_delay_ms(&phase);
        if (delay == 16)
            sixteen++;
        else if (delay == 17)
            seventeen++;
        else {
            check(false, "pacing emits only 16 or 17 ms");
            return;
        }
        total += delay;
    }

    check_u32("pacing 16 ms count", sixteen, TINYOS_GAME_PACING_16MS);
    check_u32("pacing 17 ms count", seventeen,
              TINYOS_GAME_PACING_WINDOW - TINYOS_GAME_PACING_16MS);
    check_u32("pacing total over 10000 frames", (uint32_t)total, 167427u);
    double fps = 1000.0 / ((double)total / TINYOS_GAME_PACING_WINDOW);
    check(fabs(fps - 59.7275) < 0.0001, "pacing average is 59.7275 Hz");

    uint64_t samples = tinyos_game_audio_samples_for_frames(10000);
    long double ideal = (long double)TINYOS_GAME_SAMPLE_RATE * 10000.0L *
                        TINYOS_GAME_FPS_DEN / TINYOS_GAME_FPS_NUM;
    uint64_t rounded = (uint64_t)(ideal + 0.5L);
    check(samples == rounded, "audio sample count rounds the exact ratio");
    check_u32("zero frames produce zero samples",
              (uint32_t)tinyos_game_audio_samples_for_frames(0), 0);

    uint64_t left = samples * TINYOS_GAME_FPS_NUM;
    uint64_t right = (uint64_t)TINYOS_GAME_SAMPLE_RATE * 10000u *
                     TINYOS_GAME_FPS_DEN;
    uint64_t difference = left > right ? left - right : right - left;
    check(difference <= TINYOS_GAME_FPS_NUM / 2,
          "audio reference and rational FPS stay consistent");
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    const char *root = argc > 1 ? argv[1] : "build/emulator-selftest/roms";
    printf("emulator selftest: ROM root %s\n", root);

    for (size_t i = 0; i < sizeof(systems) / sizeof(systems[0]); i++) {
        struct stat info;
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", root, systems[i].file);
        check(stat(path, &info) == 0 && info.st_size == 32768,
              "generated test ROM is 32768 bytes");
        int failures_before = failures;
        run_geometry_test(root, &systems[i]);
        run_save_test(root, &systems[i]);
        printf("emulator selftest: %s %s\n", systems[i].name,
               failures == failures_before ? "PASS" : "FAIL");
    }

    run_fbdev_test(root);
    run_input_test();
    run_gesture_test();
    run_pacing_test();

    if (failures) {
        printf("emulator selftest: %d failure(s) in %d checks\n", failures,
               checks);
        return 1;
    }
    printf("emulator selftest: %d checks, all PASS\n", checks);
    return 0;
}
