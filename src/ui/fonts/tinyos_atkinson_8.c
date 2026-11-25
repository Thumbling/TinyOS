/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: --font third_party/fonts/src/atkinson.ttf --size 8 --bpp 4 --format lvgl --lv-font-name tinyos_atkinson_8 --lv-include lvgl/lvgl.h -r 0x20-0x7E --force-fast-kern-format --no-compress -o src/ui/fonts/tinyos_atkinson_8.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef TINYOS_ATKINSON_8
#define TINYOS_ATKINSON_8 1
#endif

#if TINYOS_ATKINSON_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x37, 0x37, 0x26, 0x14, 0x47,

    /* U+0022 "\"" */
    0x98, 0x5, 0x50,

    /* U+0023 "#" */
    0x4, 0x69, 0x3, 0xcb, 0xe9, 0x8, 0x1a, 0x6,
    0xda, 0xd6, 0xa, 0x37, 0x0,

    /* U+0024 "$" */
    0x5, 0xa2, 0x7, 0x78, 0x70, 0x69, 0x70, 0x0,
    0x4b, 0xb1, 0x31, 0x66, 0x63, 0xbc, 0xa1, 0x0,
    0x30, 0x0,

    /* U+0025 "%" */
    0x4a, 0x90, 0x28, 0x0, 0x90, 0x82, 0xa0, 0x0,
    0x29, 0x79, 0x5a, 0x60, 0x0, 0x65, 0xa0, 0x90,
    0x2, 0x80, 0x58, 0x80,

    /* U+0026 "&" */
    0x9, 0x99, 0x0, 0x9, 0x17, 0x0, 0x49, 0xa4,
    0xa0, 0xa1, 0xc, 0x80, 0x3a, 0x98, 0xb2,

    /* U+0027 "'" */
    0x95,

    /* U+0028 "(" */
    0x8, 0x6, 0x30, 0x80, 0x8, 0x0, 0x44, 0x0,
    0x90,

    /* U+0029 ")" */
    0x35, 0x9, 0x9, 0x9, 0x9, 0x53,

    /* U+002A "*" */
    0x29, 0x40, 0xb6, 0x0, 0x0,

    /* U+002B "+" */
    0x0, 0xa0, 0x0, 0xa, 0x0, 0x5a, 0xda, 0x30,
    0xa, 0x0,

    /* U+002C "," */
    0x83, 0x60,

    /* U+002D "-" */
    0x7a, 0x60,

    /* U+002E "." */
    0x0, 0x83,

    /* U+002F "/" */
    0x0, 0xa0, 0x64, 0xa, 0x4, 0x60, 0xa0, 0x0,

    /* U+0030 "0" */
    0xa, 0x9a, 0x15, 0xc3, 0x38, 0x73, 0x91, 0xa5,
    0x61, 0x98, 0x9, 0x9a, 0x10,

    /* U+0031 "1" */
    0x5a, 0x70, 0x37, 0x3, 0x70, 0x37, 0x3, 0x70,

    /* U+0032 "2" */
    0x2a, 0x98, 0x2, 0x10, 0xa0, 0x0, 0x56, 0x0,
    0x66, 0x0, 0x9c, 0x99, 0x10,

    /* U+0033 "3" */
    0x2a, 0x98, 0x2, 0x10, 0xb0, 0x2, 0xc8, 0x2,
    0x0, 0xb0, 0x3a, 0x98, 0x0,

    /* U+0034 "4" */
    0x0, 0x7a, 0x0, 0x47, 0xa0, 0x28, 0xa, 0x8,
    0xaa, 0xd6, 0x0, 0xa, 0x0,

    /* U+0035 "5" */
    0x2c, 0xa9, 0x5, 0xb9, 0x50, 0x12, 0xb, 0x3,
    0x10, 0xa1, 0x2a, 0x97, 0x0,

    /* U+0036 "6" */
    0x8, 0x87, 0x5, 0x99, 0x60, 0x76, 0x9, 0x26,
    0x50, 0x83, 0xa, 0x98, 0x0,

    /* U+0037 "7" */
    0x8a, 0xac, 0x0, 0x65, 0x0, 0xb0, 0x7, 0x50,
    0xc, 0x0,

    /* U+0038 "8" */
    0x7, 0x97, 0x0, 0xa0, 0xa0, 0xc, 0xcb, 0x7,
    0x40, 0x66, 0x2a, 0x9a, 0x10,

    /* U+0039 "9" */
    0xa, 0xa9, 0x6, 0x40, 0x83, 0x56, 0x9, 0x10,
    0x8c, 0x80, 0x0, 0x90, 0x0,

    /* U+003A ":" */
    0x83, 0x0, 0x0, 0x83,

    /* U+003B ";" */
    0x83, 0x0, 0x0, 0x83, 0x60,

    /* U+003C "<" */
    0x0, 0x3a, 0x14, 0xa7, 0x10, 0x5a, 0x60, 0x0,
    0x4, 0xa1,

    /* U+003D "=" */
    0x5a, 0xaa, 0x35, 0xaa, 0xa4,

    /* U+003E ">" */
    0x76, 0x0, 0x0, 0x4a, 0x80, 0x3, 0x99, 0x7,
    0x71, 0x0,

    /* U+003F "?" */
    0x4a, 0x96, 0x20, 0x1a, 0x3, 0x91, 0x4, 0x10,
    0x8, 0x30,

    /* U+0040 "@" */
    0x6, 0x77, 0x80, 0x44, 0xab, 0x48, 0x73, 0x77,
    0x48, 0x54, 0x98, 0x94, 0x6, 0x78, 0x20,

    /* U+0041 "A" */
    0x1, 0xf2, 0x0, 0x85, 0x80, 0xa, 0xb, 0x5,
    0xca, 0xc5, 0xb1, 0x1, 0xb0,

    /* U+0042 "B" */
    0x6c, 0x9b, 0x26, 0x50, 0x65, 0x6b, 0x9e, 0x26,
    0x50, 0x38, 0x6c, 0x9a, 0x30,

    /* U+0043 "C" */
    0x9, 0xab, 0x56, 0x70, 0x3, 0x93, 0x0, 0x6,
    0x70, 0x3, 0x9, 0xaa, 0x60,

    /* U+0044 "D" */
    0x6c, 0x9a, 0x26, 0x50, 0x1b, 0x65, 0x0, 0xc6,
    0x50, 0x1b, 0x6c, 0x9a, 0x20,

    /* U+0045 "E" */
    0x6c, 0xaa, 0x26, 0x50, 0x0, 0x6c, 0xa4, 0x6,
    0x50, 0x0, 0x6c, 0xaa, 0x20,

    /* U+0046 "F" */
    0x6c, 0xaa, 0x6, 0x50, 0x0, 0x6c, 0xaa, 0x6,
    0x50, 0x0, 0x65, 0x0, 0x0,

    /* U+0047 "G" */
    0x9, 0xab, 0x60, 0x67, 0x0, 0x20, 0x93, 0x1a,
    0xb1, 0x67, 0x0, 0xb2, 0x9, 0xaa, 0xa2,

    /* U+0048 "H" */
    0x65, 0x0, 0xc6, 0x50, 0xc, 0x6c, 0xaa, 0xd6,
    0x50, 0xc, 0x65, 0x0, 0xc0,

    /* U+0049 "I" */
    0x4e, 0x70, 0xb0, 0xb, 0x0, 0xb0, 0x4e, 0x70,

    /* U+004A "J" */
    0x0, 0x47, 0x0, 0x47, 0x0, 0x47, 0x60, 0x56,
    0x6a, 0xb1,

    /* U+004B "K" */
    0x65, 0xa, 0x46, 0x59, 0x40, 0x6d, 0xc1, 0x6,
    0x73, 0xb0, 0x65, 0x6, 0x80,

    /* U+004C "L" */
    0x65, 0x0, 0x6, 0x50, 0x0, 0x65, 0x0, 0x6,
    0x50, 0x0, 0x6c, 0xaa, 0x20,

    /* U+004D "M" */
    0x6f, 0x0, 0x7f, 0x6b, 0x60, 0xbc, 0x66, 0xa2,
    0x8c, 0x65, 0x99, 0x3c, 0x65, 0x4d, 0xc,

    /* U+004E "N" */
    0x6e, 0x10, 0xc6, 0x99, 0xc, 0x65, 0x92, 0xc6,
    0x51, 0xac, 0x65, 0x7, 0xe0,

    /* U+004F "O" */
    0x8, 0xab, 0x60, 0x67, 0x0, 0xa3, 0x93, 0x0,
    0x66, 0x67, 0x0, 0xa3, 0x8, 0xab, 0x60,

    /* U+0050 "P" */
    0x6c, 0x9b, 0x26, 0x50, 0x57, 0x6c, 0x99, 0x16,
    0x50, 0x0, 0x65, 0x0, 0x0,

    /* U+0051 "Q" */
    0x8, 0xab, 0x60, 0x6, 0x70, 0xa, 0x30, 0x93,
    0x16, 0x66, 0x6, 0x70, 0x7d, 0x30, 0x8, 0xaa,
    0x9a, 0x0, 0x0, 0x0, 0x10,

    /* U+0052 "R" */
    0x6c, 0x9a, 0x36, 0x50, 0x57, 0x6c, 0xca, 0x16,
    0x55, 0x90, 0x65, 0x9, 0x50,

    /* U+0053 "S" */
    0x3a, 0xa9, 0x8, 0x40, 0x0, 0x7, 0x98, 0x2,
    0x0, 0x75, 0x3a, 0xaa, 0x10,

    /* U+0054 "T" */
    0x9b, 0xda, 0x40, 0x29, 0x0, 0x2, 0x90, 0x0,
    0x29, 0x0, 0x2, 0x90, 0x0,

    /* U+0055 "U" */
    0x65, 0x0, 0xc6, 0x50, 0xc, 0x65, 0x0, 0xb4,
    0x70, 0xb, 0xa, 0xab, 0x40,

    /* U+0056 "V" */
    0xb1, 0x5, 0x76, 0x60, 0xb1, 0xb, 0x1b, 0x0,
    0xa8, 0x50, 0x4, 0xe0, 0x0,

    /* U+0057 "W" */
    0xc0, 0x3e, 0x5, 0x88, 0x47, 0xb2, 0x93, 0x48,
    0x94, 0x6c, 0x0, 0xb9, 0xa, 0xb0, 0xb, 0x70,
    0xb6, 0x0,

    /* U+0058 "X" */
    0x67, 0x7, 0x60, 0xa7, 0xa0, 0x2, 0xf2, 0x0,
    0xb5, 0xb0, 0x85, 0x6, 0x80,

    /* U+0059 "Y" */
    0x93, 0x7, 0x51, 0xb1, 0xb0, 0x5, 0xd2, 0x0,
    0xc, 0x0, 0x0, 0xc0, 0x0,

    /* U+005A "Z" */
    0x5a, 0xae, 0x50, 0x3, 0xa0, 0x2, 0xb0, 0x1,
    0xb1, 0x0, 0x9c, 0xaa, 0x50,

    /* U+005B "[" */
    0x6a, 0x26, 0x40, 0x64, 0x6, 0x40, 0x64, 0x6,
    0xb2,

    /* U+005C "\\" */
    0xa0, 0x4, 0x60, 0xa, 0x0, 0x64, 0x0, 0xa0,

    /* U+005D "]" */
    0x6c, 0xa, 0xa, 0xa, 0xa, 0x6c,

    /* U+005E "^" */
    0x5, 0xb0, 0xb, 0x73, 0x56, 0xa,

    /* U+005F "_" */
    0x9a, 0xa0,

    /* U+0060 "`" */
    0x54,

    /* U+0061 "a" */
    0x29, 0x94, 0x3, 0x6a, 0x86, 0x2a, 0x6a, 0x8a,

    /* U+0062 "b" */
    0x73, 0x0, 0x7, 0xa9, 0x80, 0x74, 0xa, 0x17,
    0x40, 0xa1, 0x79, 0x98, 0x0,

    /* U+0063 "c" */
    0x2a, 0xa5, 0x92, 0x0, 0x92, 0x0, 0x2a, 0x95,

    /* U+0064 "d" */
    0x0, 0xa, 0x2, 0xa8, 0xc0, 0x92, 0xc, 0x9,
    0x20, 0xc0, 0x2a, 0x8c, 0x0,

    /* U+0065 "e" */
    0x28, 0x97, 0x9, 0x98, 0xa0, 0x93, 0x0, 0x2,
    0xb9, 0x60,

    /* U+0066 "f" */
    0x2b, 0x28, 0xc2, 0x37, 0x3, 0x70, 0x37, 0x0,

    /* U+0067 "g" */
    0x2a, 0x8b, 0x92, 0xc, 0x91, 0xc, 0x2a, 0x8c,
    0x12, 0xa, 0x9, 0x94,

    /* U+0068 "h" */
    0x73, 0x0, 0x79, 0x97, 0x74, 0xb, 0x73, 0xa,
    0x73, 0xa,

    /* U+0069 "i" */
    0x29, 0x69, 0x19, 0x19, 0x19,

    /* U+006A "j" */
    0x4, 0x60, 0x46, 0x4, 0x60, 0x46, 0x4, 0x64,
    0xb2,

    /* U+006B "k" */
    0x73, 0x0, 0x73, 0x93, 0x7b, 0x60, 0x78, 0xa0,
    0x73, 0x57,

    /* U+006C "l" */
    0x73, 0x73, 0x73, 0x73, 0x59,

    /* U+006D "m" */
    0x78, 0x99, 0x8b, 0x7, 0x40, 0xb0, 0x82, 0x73,
    0xa, 0x7, 0x37, 0x30, 0xa0, 0x73,

    /* U+006E "n" */
    0x79, 0x97, 0x74, 0xa, 0x73, 0xa, 0x73, 0xa,

    /* U+006F "o" */
    0x2a, 0xa6, 0x9, 0x20, 0xb0, 0x92, 0xb, 0x2,
    0xaa, 0x70,

    /* U+0070 "p" */
    0x78, 0x88, 0x7, 0x40, 0x91, 0x74, 0xa, 0x17,
    0x99, 0x80, 0x73, 0x0, 0x0,

    /* U+0071 "q" */
    0x2a, 0x8b, 0x0, 0x92, 0xc, 0x0, 0x92, 0xc,
    0x0, 0x2a, 0x8d, 0x0, 0x0, 0x8, 0xa0,

    /* U+0072 "r" */
    0x79, 0x67, 0x50, 0x73, 0x7, 0x30,

    /* U+0073 "s" */
    0x5a, 0xa1, 0x78, 0x30, 0x12, 0x76, 0x5a, 0xa3,

    /* U+0074 "t" */
    0x28, 0x8, 0xc2, 0x28, 0x2, 0x80, 0x1c, 0x20,

    /* U+0075 "u" */
    0x73, 0xa, 0x73, 0xa, 0x64, 0xb, 0x2b, 0x8a,

    /* U+0076 "v" */
    0xb0, 0x55, 0x73, 0xa0, 0x19, 0x90, 0xc, 0x40,

    /* U+0077 "w" */
    0xb0, 0xd1, 0x91, 0x93, 0x95, 0xa0, 0x4c, 0x2a,
    0x80, 0xd, 0xa, 0x30,

    /* U+0078 "x" */
    0x83, 0x84, 0xb, 0x80, 0xb, 0x90, 0x92, 0x75,

    /* U+0079 "y" */
    0xb0, 0x74, 0x64, 0xb0, 0x1b, 0x80, 0xa, 0x20,
    0x69, 0x0,

    /* U+007A "z" */
    0x59, 0xd4, 0x2, 0x90, 0x1a, 0x0, 0x9b, 0x94,

    /* U+007B "{" */
    0x1c, 0x22, 0x80, 0x84, 0x2, 0x80, 0x28, 0x0,
    0xb2,

    /* U+007C "|" */
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64,

    /* U+007D "}" */
    0x69, 0x0, 0xa0, 0xa, 0x20, 0xa0, 0xa, 0x7,
    0x80,

    /* U+007E "~" */
    0x38, 0x97, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 36, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 39, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 8, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 39, .adv_w = 119, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 90, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 20, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 75, .adv_w = 38, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 84, .adv_w = 38, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 90, .adv_w = 55, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 95, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 26, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 107, .adv_w = 47, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 109, .adv_w = 26, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 48, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 132, .adv_w = 51, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 166, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 179, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 192, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 26, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 26, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 250, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 260, .adv_w = 77, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 265, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 67, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 100, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 300, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 86, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 352, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 378, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 393, .adv_w = 88, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 406, .adv_w = 53, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 437, .adv_w = 69, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 105, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 465, .adv_w = 88, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 493, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 96, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 527, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 540, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 553, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 566, .adv_w = 88, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 579, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 592, .adv_w = 108, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 610, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 636, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 649, .adv_w = 40, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 658, .adv_w = 48, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 666, .adv_w = 40, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 672, .adv_w = 71, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 678, .adv_w = 51, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 680, .adv_w = 34, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 681, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 702, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 710, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 723, .adv_w = 69, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 733, .adv_w = 40, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 741, .adv_w = 72, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 753, .adv_w = 70, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 763, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 768, .adv_w = 33, .box_w = 3, .box_h = 6, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 777, .adv_w = 63, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 787, .adv_w = 31, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 792, .adv_w = 108, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 806, .adv_w = 70, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 814, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 824, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 837, .adv_w = 74, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 852, .adv_w = 44, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 858, .adv_w = 60, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 866, .adv_w = 41, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 874, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 882, .adv_w = 56, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 890, .adv_w = 84, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 902, .adv_w = 59, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 910, .adv_w = 55, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 920, .adv_w = 59, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 928, .adv_w = 41, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 937, .adv_w = 30, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 943, .adv_w = 41, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 952, .adv_w = 71, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 2, 3, 2,
    4, 5, 6, 7, 0, 8, 9, 10,
    11, 12, 13, 0, 0, 0, 0, 0,
    0, 0, 14, 15, 16, 17, 18, 19,
    20, 0, 21, 22, 23, 24, 0, 0,
    17, 25, 26, 27, 28, 29, 22, 30,
    31, 32, 33, 34, 0, 0, 0, 0,
    0, 0, 0, 0, 35, 0, 36, 37,
    38, 39, 0, 40, 41, 42, 39, 39,
    0, 0, 43, 44, 45, 46, 47, 48,
    49, 50, 48, 51, 0, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 2, 0, 0, 3, 4, 3,
    5, 0, 6, 7, 0, 8, 9, 0,
    10, 11, 12, 0, 0, 0, 0, 0,
    0, 0, 13, 0, 14, 0, 0, 0,
    14, 0, 15, 16, 0, 0, 0, 0,
    14, 0, 14, 0, 0, 17, 18, 19,
    20, 21, 22, 23, 0, 0, 0, 0,
    0, 0, 24, 0, 25, 25, 25, 26,
    27, 0, 28, 29, 0, 30, 31, 31,
    25, 32, 25, 33, 34, 35, 36, 37,
    38, 39, 37, 40, 0, 0, 0, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, -4, 4,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -12, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    4, 0, 0, -7, 0, 4, 0, -6,
    0, 5, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, -4, 0, 0,
    0, -2, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -2, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, -5, 0, 0,
    -8, -4, -9, -6, 0, -8, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, -3, -3, -5, -6, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, -2, 0, -2, -4, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -4, 0,
    -5, 0, -4, -2, -2, -6, -2, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, -4,
    3, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, -4, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -6, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, -5, 0, 4,
    -13, -3, -8, -5, 4, -11, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, -5, -6, 4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -10, 0, 0, -9,
    0, 0, 0, 0, -4, -2, -2, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, 0, 0, 0,
    0, 0, 0, 0, 2, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, -3, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -8, -5, 0, -13,
    3, 0, 5, 4, 0, 2, 0, -7,
    -11, 2, -9, 0, 0, 0, -8, -7,
    -7, -5, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -9, -4, 0, -5,
    5, 0, 2, 2, 0, 2, 0, -5,
    -6, 0, -6, 0, 0, 0, -2, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -6, -2, 0, -4,
    4, 0, 2, 0, 0, 0, 0, -4,
    -3, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -8, -6, 0, -7,
    2, 0, 2, 0, 0, 0, 0, -10,
    -11, 0, -11, 0, 0, 0, 0, 0,
    0, -7, 0, -5, 0, 0, -2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, -4, 0, -10, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -8, 0, -5, -4, -2, -10, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    4, 0, 0, 2, 0, 0, 0, -4,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -7, 0, 0, -4, 0, -8, 0, 0,
    -1, 0, -2, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 3,
    0, 0, 0, 0, 2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 0, 0, -2,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 3, 3, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    -5, 0, -2, 0, 0, -7, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -9, 0, 0, 0, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -7, 0, 0, 0,
    0, 0, 0, 0, -2, 0, -2, -2,
    0, 3, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 51,
    .right_class_cnt     = 40,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t tinyos_atkinson_8 = {
#else
lv_font_t tinyos_atkinson_8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 8,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .static_bitmap = 1,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if TINYOS_ATKINSON_8*/

