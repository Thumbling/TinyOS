#include "ui/theme.h"

#include <stddef.h>
#include <string.h>

tinyos_theme_t tinyos_theme;

/*
 * Twenty-five hand-tuned palettes. Every text colour is checked against the
 * surface colours for at least 4.5:1 contrast; the accent is used for focus and
 * highlights only. Wallpaper is the default style index for the palette.
 */
static const tinyos_theme_definition_t definitions[TINYOS_THEME_COUNT] = {
    {"Abyss",        0x070B10, 0x0B1520, 0x101B25, 0x162632, 0x1D3140, 0xF2F8FB, 0xA9BECC, 0x8FA6B5, 0x3DE1FF, 0x9A86FF, 0x48E6A0, 0xFFB454, 0xFF7070, 0x041014, 0},
    {"Aurora",       0x061016, 0x0A1B22, 0x0E2029, 0x15303A, 0x1C4350, 0xEEFBF7, 0xA6C7C0, 0x86AAA4, 0x4FF0C8, 0x7AA7FF, 0x66F5A8, 0xFFCF6E, 0xFF7A8A, 0x032018, 0},
    {"Ember",        0x120A08, 0x1D0F0B, 0x261512, 0x34201A, 0x4A2B22, 0xFFF3EA, 0xD0B3A4, 0xB59787, 0xFF9D5C, 0xFF5D73, 0x9BE36D, 0xFFC857, 0xFF6B6B, 0x1A0904, 2},
    {"Rosewood",     0x120812, 0x1D0C1A, 0x271222, 0x351A2D, 0x4B2A40, 0xFCEBF4, 0xC9A9BC, 0xAD8CA0, 0xFF8FC7, 0xC69BFF, 0x72E6B0, 0xFFCB72, 0xFF6F91, 0x1A0712, 1},
    {"Cobalt",       0x050B1A, 0x09152E, 0x0E1E3D, 0x142B51, 0x1E3D6B, 0xEDF4FF, 0xA7B9D6, 0x8599BA, 0x62A8FF, 0x9C9CFF, 0x5FE3B2, 0xFFC76B, 0xFF7C8A, 0x031024, 3},
    {"Lavender",     0x0C0818, 0x16102B, 0x1E1738, 0x2B2150, 0x3D3069, 0xF4F1FF, 0xB9AFD8, 0x9A8EBE, 0xB79CFF, 0xFF9EDB, 0x7CE3B0, 0xFFD17A, 0xFF7D9B, 0x130A2A, 1},
    {"Forest",       0x06110B, 0x0A1B12, 0x0E2419, 0x153322, 0x1F4A31, 0xEDFBF1, 0xA9C9B4, 0x87A995, 0x61E08A, 0xC5E86C, 0x61E08A, 0xFFD166, 0xFF7F7F, 0x04160A, 4},
    {"Sandstone",    0x151009, 0x21190F, 0x2B2116, 0x3A2C1D, 0x54412C, 0xFBF3E7, 0xC9B79E, 0xA9977F, 0xF0B35B, 0xD98B6A, 0x9BD37A, 0xFFD27C, 0xF27C7C, 0x1A1006, 2},
    {"Nord",         0x11161D, 0x1A222C, 0x222C38, 0x2D3A47, 0x3D4C5C, 0xEAF2F8, 0xB0C2D0, 0x93A7B8, 0x88C0D0, 0xD3A9CC, 0x9FD98F, 0xEBCB8B, 0xF0A6B8, 0x0D151A, 5},
    {"Gruvbox",      0x1D2021, 0x282828, 0x32302F, 0x3C3836, 0x504945, 0xFBF1C7, 0xC7B99A, 0xBDB096, 0xFABD2F, 0xFF8A7A, 0xB8BB26, 0xFABD2F, 0xFF9A8A, 0x1D2021, 6},
    {"Dracula",      0x191A21, 0x21222C, 0x282A36, 0x343746, 0x44475A, 0xF8F8F2, 0xBFC2D0, 0xAFB2C5, 0xBD93F9, 0xFF79C6, 0x50FA7B, 0xF1FA8C, 0xFF7C7C, 0x1A1326, 1},
    {"Tokyo Night",  0x0F111A, 0x161925, 0x1A1B26, 0x24283B, 0x353B52, 0xC0CAF5, 0x9AA5CE, 0x97A2CC, 0x7AA2F7, 0xBB9AF7, 0x9ECE6A, 0xE0AF68, 0xF7768E, 0x0A0E1A, 3},
    {"Solarized",    0x002B36, 0x073642, 0x0A3A45, 0x123F4A, 0x1C5260, 0xFDF6E3, 0xC9BFA5, 0xB9B09B, 0x4FD1C5, 0x9FA8FF, 0xB8D34A, 0xE3B341, 0xFF8A8A, 0x002B36, 4},
    {"Monochrome",   0x050505, 0x0D0D0D, 0x151515, 0x202020, 0x343434, 0xF5F5F5, 0xB8B8B8, 0x949494, 0xF5F5F5, 0xAFAFAF, 0xD0D0D0, 0xE0E0E0, 0xFFFFFF, 0x050505, 9},
    {"Paper",        0xF3EFE7, 0xE8E1D4, 0xFBF8F1, 0xF1EADD, 0xD8CEBC, 0x1C2328, 0x4E5A61, 0x4F5C63, 0x0B6E8C, 0x6B4FA1, 0x1C6B47, 0x8A5A00, 0xA32631, 0xFFFFFF, 8},
    {"Porcelain",    0xEEF3F6, 0xE0E9EF, 0xF8FBFC, 0xECF2F5, 0xC8D6DF, 0x17232B, 0x4B5D68, 0x52646E, 0x166C9C, 0x5E5AA8, 0x1C6A4A, 0x7A5600, 0xA02A3A, 0xFFFFFF, 7},
    {"Mint Light",   0xEAF4ED, 0xD9EADD, 0xF7FCF8, 0xEAF4EC, 0xC2D9C8, 0x15251B, 0x496254, 0x5C7366, 0x167A56, 0x3C6B8F, 0x1A7A4A, 0x7B5A00, 0xA02A2A, 0xFFFFFF, 8},
    {"Solar Flare",  0xFAF0DF, 0xF2DFC4, 0xFFF8EC, 0xF8EAD6, 0xE0C9A7, 0x2A1B12, 0x6B5140, 0x6D5548, 0x9E4308, 0x8A3410, 0x3C6B2F, 0x8A5700, 0xA02A2A, 0xFFFFFF, 2},
    {"Cyberpunk",    0x0A0614, 0x170A22, 0x1D0E2C, 0x2C1442, 0x4A2066, 0xFFF1FB, 0xCFB4DC, 0xB092C2, 0xFF3DCE, 0x3DF5FF, 0x54F5B0, 0xFFE05C, 0xFF4D6D, 0x1A0416, 6},
    {"Vaporwave",    0x0B0818, 0x171034, 0x211545, 0x2F1D5C, 0x46317A, 0xF5EEFF, 0xC8B6E8, 0xA48DC8, 0xFF71CE, 0x01CDFE, 0x05FFA1, 0xFFCC70, 0xFF5F8F, 0x180821, 1},
    {"Terminal",     0x030603, 0x071008, 0x0A160D, 0x102515, 0x1B4226, 0xE6FFEA, 0xA2C9AA, 0x7FA689, 0x43FF7A, 0x8DFF9E, 0x43FF7A, 0xE8FF6A, 0xFF6B6B, 0x021006, 4},
    {"Amber CRT",    0x0C0803, 0x171006, 0x1E1509, 0x2B1F0E, 0x493514, 0xFFF0CC, 0xD6B77D, 0xB4945E, 0xFFBF4D, 0xFF8C42, 0xD8E36A, 0xFFCF4D, 0xFF6B57, 0x1A0D02, 2},
    {"Ice",          0x071218, 0x0B202A, 0x102B35, 0x173C48, 0x245866, 0xE9FAFF, 0xA9CED9, 0x87ADBA, 0x7FE8FF, 0xA6C8FF, 0x7CF0C0, 0xF7D98A, 0xFF8A9A, 0x04161C, 3},
    {"Cosmos",       0x05040E, 0x0C0A1E, 0x12102A, 0x1D193D, 0x332B60, 0xF0ECFF, 0xB2A9D8, 0x9188BC, 0x9C8BFF, 0xF07BFF, 0x6EE7D2, 0xFFD37A, 0xFF7597, 0x0B0720, 5},
    {"Candy",        0x160A14, 0x251020, 0x33172D, 0x431F3A, 0x633352, 0xFFEDF7, 0xD6B2C8, 0xB893AA, 0xFF8FCB, 0x8CF0D0, 0x8CF0B0, 0xFFD27A, 0xFF6F91, 0x1A0714, 7},
};

static bool styles_ready;

/* Focus and press are state changes: animate the ring and the fill, never the
 * layout. 90 ms sits inside the 80-120 ms focus budget for a 240 px screen. */
static const lv_style_prop_t ui_transition_props[] = {
    LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH, LV_STYLE_BORDER_OPA,
    LV_STYLE_BG_COLOR, LV_STYLE_BG_OPA, LV_STYLE_TEXT_COLOR, 0
};
static lv_style_transition_dsc_t ui_transition;
static bool transition_ready;

uint32_t tinyos_color_mix(uint32_t a, uint32_t b, uint8_t ratio_b)
{
    uint32_t ar = (a >> 16) & 0xFFu, ag = (a >> 8) & 0xFFu, ab = a & 0xFFu;
    uint32_t br = (b >> 16) & 0xFFu, bg = (b >> 8) & 0xFFu, bb = b & 0xFFu;
    uint32_t rr = (ar * (255u - ratio_b) + br * ratio_b) / 255u;
    uint32_t rg = (ag * (255u - ratio_b) + bg * ratio_b) / 255u;
    uint32_t rb = (ab * (255u - ratio_b) + bb * ratio_b) / 255u;
    return (rr << 16) | (rg << 8) | rb;
}

static void style_reset(lv_style_t *style)
{
    if (styles_ready) {
        lv_style_reset(style);
    } else {
        lv_style_init(style);
    }
}

static void build_styles(void)
{
    style_reset(&tinyos_theme.card);
    lv_style_set_bg_color(&tinyos_theme.card, lv_color_hex(TINYOS_COLOR_SURFACE));
    lv_style_set_bg_opa(&tinyos_theme.card, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.card, 20);
    lv_style_set_border_width(&tinyos_theme.card, 1);
    lv_style_set_border_color(&tinyos_theme.card, lv_color_hex(TINYOS_COLOR_LINE));
    lv_style_set_border_opa(&tinyos_theme.card, LV_OPA_70);
    lv_style_set_shadow_width(&tinyos_theme.card, 18);
    lv_style_set_shadow_color(&tinyos_theme.card, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&tinyos_theme.card, LV_OPA_20);
    lv_style_set_shadow_offset_y(&tinyos_theme.card, 7);
    lv_style_set_pad_all(&tinyos_theme.card, 0);

    style_reset(&tinyos_theme.tile);
    lv_style_set_bg_color(&tinyos_theme.tile, lv_color_hex(TINYOS_COLOR_SURFACE));
    lv_style_set_bg_opa(&tinyos_theme.tile, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.tile, 16);
    lv_style_set_border_width(&tinyos_theme.tile, 1);
    lv_style_set_border_color(&tinyos_theme.tile, lv_color_hex(TINYOS_COLOR_LINE));
    lv_style_set_border_opa(&tinyos_theme.tile, LV_OPA_60);
    lv_style_set_shadow_width(&tinyos_theme.tile, 12);
    lv_style_set_shadow_color(&tinyos_theme.tile, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&tinyos_theme.tile, LV_OPA_20);
    lv_style_set_shadow_offset_y(&tinyos_theme.tile, 4);
    lv_style_set_pad_all(&tinyos_theme.tile, 0);

    if (!transition_ready) {
        lv_style_transition_dsc_init(&ui_transition, ui_transition_props,
                                     lv_anim_path_ease_out, 90, 0, NULL);
        transition_ready = true;
    }

    style_reset(&tinyos_theme.focus);
    lv_style_set_border_width(&tinyos_theme.focus, 2);
    lv_style_set_border_color(&tinyos_theme.focus, lv_color_hex(TINYOS_COLOR_CYAN));
    lv_style_set_border_opa(&tinyos_theme.focus, LV_OPA_COVER);
    lv_style_set_outline_width(&tinyos_theme.focus, 0);
    lv_style_set_transition(&tinyos_theme.focus, &ui_transition);

    /* A pressed control flashes to the primary text colour and settles back:
     * immediate acknowledgement without a scale or a shadow layer. */
    style_reset(&tinyos_theme.pressed);
    lv_style_set_border_width(&tinyos_theme.pressed, 2);
    lv_style_set_border_color(&tinyos_theme.pressed, lv_color_hex(TINYOS_COLOR_TEXT));
    lv_style_set_border_opa(&tinyos_theme.pressed, LV_OPA_COVER);
    lv_style_set_transition(&tinyos_theme.pressed, &ui_transition);

    style_reset(&tinyos_theme.nav);
    lv_style_set_bg_color(&tinyos_theme.nav, lv_color_hex(TINYOS_COLOR_BG_2));
    lv_style_set_bg_opa(&tinyos_theme.nav, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.nav, 14);
    lv_style_set_border_width(&tinyos_theme.nav, 1);
    lv_style_set_border_color(&tinyos_theme.nav, lv_color_hex(TINYOS_COLOR_LINE));
    lv_style_set_border_opa(&tinyos_theme.nav, LV_OPA_80);
    lv_style_set_shadow_width(&tinyos_theme.nav, 14);
    lv_style_set_shadow_color(&tinyos_theme.nav, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&tinyos_theme.nav, LV_OPA_30);
    lv_style_set_shadow_offset_y(&tinyos_theme.nav, 4);

    style_reset(&tinyos_theme.nav_item);
    lv_style_set_bg_opa(&tinyos_theme.nav_item, LV_OPA_TRANSP);
    lv_style_set_border_width(&tinyos_theme.nav_item, 0);
    lv_style_set_transition(&tinyos_theme.nav_item, &ui_transition);

    style_reset(&tinyos_theme.nav_active);
    lv_style_set_bg_color(&tinyos_theme.nav_active, lv_color_hex(TINYOS_COLOR_SURFACE_2));
    lv_style_set_bg_opa(&tinyos_theme.nav_active, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.nav_active, 11);
    lv_style_set_border_width(&tinyos_theme.nav_active, 0);

    style_reset(&tinyos_theme.row);
    lv_style_set_bg_color(&tinyos_theme.row, lv_color_hex(TINYOS_COLOR_SURFACE));
    lv_style_set_bg_opa(&tinyos_theme.row, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.row, 12);
    lv_style_set_border_width(&tinyos_theme.row, 0);
    lv_style_set_pad_all(&tinyos_theme.row, 0);

    style_reset(&tinyos_theme.pill);
    lv_style_set_bg_color(&tinyos_theme.pill, lv_color_hex(TINYOS_COLOR_SURFACE_2));
    lv_style_set_bg_opa(&tinyos_theme.pill, LV_OPA_COVER);
    lv_style_set_radius(&tinyos_theme.pill, LV_RADIUS_CIRCLE);
    lv_style_set_border_width(&tinyos_theme.pill, 0);
    lv_style_set_pad_all(&tinyos_theme.pill, 0);

    styles_ready = true;
}

void tinyos_theme_apply(int index)
{
    if (index < 0 || index >= TINYOS_THEME_COUNT) {
        index = 0;
    }
    const tinyos_theme_definition_t *definition = &definitions[index];
    tinyos_theme.bg = definition->bg;
    tinyos_theme.bg_2 = definition->bg_2;
    tinyos_theme.surface = definition->surface;
    tinyos_theme.surface_2 = definition->surface_2;
    tinyos_theme.line = definition->line;
    tinyos_theme.text = definition->text;
    tinyos_theme.muted = definition->muted;
    tinyos_theme.dim = definition->dim;
    tinyos_theme.accent = definition->accent;
    tinyos_theme.accent_2 = definition->accent_2;
    tinyos_theme.accent_deep = tinyos_color_mix(definition->accent, definition->bg, 150);
    tinyos_theme.success = definition->success;
    tinyos_theme.warning = definition->warning;
    tinyos_theme.danger = definition->danger;
    tinyos_theme.on_accent = definition->on_accent;
    tinyos_theme.index = index;

    build_styles();
    lv_obj_report_style_change(NULL);
}

void tinyos_theme_init(void)
{
    tinyos_theme_apply(0);
}

int tinyos_theme_index(void)
{
    return tinyos_theme.index;
}

const tinyos_theme_definition_t *tinyos_theme_definition(int index)
{
    if (index < 0 || index >= TINYOS_THEME_COUNT) {
        index = 0;
    }
    return &definitions[index];
}

const char *tinyos_theme_short_name(int index)
{
    const char *name = tinyos_theme_name(index);
    return strcmp(name, "Monochrome") == 0 ? "Mono" : name;
}

const char *tinyos_theme_name(int index)
{
    return tinyos_theme_definition(index)->name;
}
