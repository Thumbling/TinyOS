/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: --font third_party/fonts/src/plex.ttf --size 8 --bpp 4 --format lvgl --lv-font-name tinyos_plex_8 --lv-include lvgl/lvgl.h -r 0x20-0x7E --force-fast-kern-format --no-compress -o src/ui/fonts/tinyos_plex_8.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef TINYOS_PLEX_8
#define TINYOS_PLEX_8 1
#endif

#if TINYOS_PLEX_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x37, 0x37, 0x26, 0x4, 0x37,

    /* U+0022 "\"" */
    0x54, 0x95, 0x49, 0x0, 0x10,

    /* U+0023 "#" */
    0x0, 0x73, 0x40, 0x19, 0x9b, 0x71, 0x5, 0x28,
    0x0, 0x3b, 0x7b, 0x60, 0x8, 0x8, 0x0,

    /* U+0024 "$" */
    0x3, 0xa1, 0x4, 0x99, 0x81, 0x48, 0x70, 0x0,
    0x4c, 0x90, 0x10, 0x76, 0x43, 0xac, 0xa0, 0x0,
    0x40, 0x0,

    /* U+0025 "%" */
    0x37, 0x80, 0x27, 0x7, 0x28, 0x9, 0x0, 0x17,
    0x68, 0x47, 0x40, 0x5, 0x48, 0x9, 0x2, 0x60,
    0x55, 0x70,

    /* U+0026 "&" */
    0x8, 0x88, 0x0, 0x9, 0x38, 0x0, 0x8, 0xb1,
    0x91, 0x54, 0x1a, 0x60, 0x1a, 0x99, 0xa0,

    /* U+0027 "'" */
    0x54, 0x54, 0x0,

    /* U+0028 "(" */
    0x5, 0x40, 0x90, 0x35, 0x5, 0x40, 0x35, 0x0,
    0x90, 0x5, 0x40,

    /* U+0029 ")" */
    0x8, 0x10, 0x0, 0x80, 0x0, 0x90, 0x0, 0x90,
    0x0, 0x80, 0x0, 0x80, 0x8, 0x10,

    /* U+002A "*" */
    0x17, 0x10, 0x3d, 0x91, 0x14, 0x50,

    /* U+002B "+" */
    0x0, 0x60, 0x0, 0x9, 0x0, 0x38, 0xc8, 0x20,
    0x9, 0x0,

    /* U+002C "," */
    0x47, 0x53,

    /* U+002D "-" */
    0x4a, 0x60,

    /* U+002E "." */
    0x47,

    /* U+002F "/" */
    0x0, 0x80, 0x54, 0x8, 0x0, 0x80, 0x44, 0x8,
    0x0, 0x40, 0x0,

    /* U+0030 "0" */
    0x9, 0x98, 0x5, 0x40, 0x72, 0x73, 0x6, 0x46,
    0x40, 0x72, 0x9, 0x98, 0x0,

    /* U+0031 "1" */
    0x9, 0xc0, 0x3, 0x1a, 0x0, 0x0, 0xa0, 0x0,
    0xa, 0x0, 0x29, 0xd9, 0x30,

    /* U+0032 "2" */
    0x1a, 0x98, 0x1, 0x10, 0xb0, 0x0, 0x38, 0x0,
    0x57, 0x0, 0x5d, 0x99, 0x20,

    /* U+0033 "3" */
    0x2a, 0x98, 0x0, 0x0, 0xb0, 0x4, 0xa6, 0x0,
    0x0, 0xb0, 0x49, 0x98, 0x0,

    /* U+0034 "4" */
    0x0, 0x9a, 0x0, 0x63, 0xa0, 0x37, 0xa, 0x8,
    0x88, 0xd3, 0x0, 0xa, 0x0,

    /* U+0035 "5" */
    0xc, 0x98, 0x2, 0xa8, 0x60, 0x2, 0x9, 0x20,
    0x10, 0x82, 0x1a, 0x98, 0x0,

    /* U+0036 "6" */
    0x3, 0x60, 0x0, 0xa8, 0x60, 0x55, 0x9, 0x26,
    0x40, 0x82, 0x1a, 0x98, 0x0,

    /* U+0037 "7" */
    0x79, 0x8c, 0x23, 0x10, 0xa0, 0x0, 0x64, 0x0,
    0xb, 0x0, 0x5, 0x50, 0x0,

    /* U+0038 "8" */
    0x1a, 0x9a, 0x4, 0x60, 0x91, 0xa, 0x99, 0x7,
    0x30, 0x74, 0x2a, 0x8a, 0x10,

    /* U+0039 "9" */
    0x1a, 0x98, 0x6, 0x40, 0x83, 0x19, 0x8b, 0x20,
    0x1, 0xb0, 0x2, 0xa1, 0x0,

    /* U+003A ":" */
    0x38, 0x0, 0x0, 0x38,

    /* U+003B ";" */
    0x38, 0x0, 0x0, 0x38, 0x34,

    /* U+003C "<" */
    0x0, 0x0, 0x0, 0x6, 0x90, 0x29, 0x30, 0x1,
    0x86, 0x0, 0x0, 0x39, 0x0, 0x0, 0x0,

    /* U+003D "=" */
    0x38, 0x88, 0x20, 0x0, 0x0, 0x38, 0x88, 0x20,

    /* U+003E ">" */
    0x0, 0x0, 0x1, 0x94, 0x0, 0x0, 0x59, 0x0,
    0x7, 0x70, 0x29, 0x10, 0x0, 0x0, 0x0,

    /* U+003F "?" */
    0x69, 0xa3, 0x10, 0x66, 0x9, 0x40, 0x1, 0x0,
    0x9, 0x20,

    /* U+0040 "@" */
    0x3, 0x77, 0x76, 0x2, 0x77, 0x7a, 0x44, 0x61,
    0xa0, 0x90, 0x76, 0x2a, 0x9, 0x7, 0x18, 0x57,
    0x67, 0x10, 0x17, 0x77, 0x30,

    /* U+0041 "A" */
    0x1, 0xe2, 0x0, 0x75, 0x80, 0xa, 0xa, 0x3,
    0xc9, 0xb5, 0x91, 0x0, 0xa0,

    /* U+0042 "B" */
    0x4b, 0x9b, 0x34, 0x60, 0x47, 0x4b, 0x8c, 0x34,
    0x60, 0x1a, 0x4b, 0x9a, 0x50,

    /* U+0043 "C" */
    0x9, 0x9a, 0x25, 0x50, 0x2, 0x73, 0x0, 0x5,
    0x50, 0x3, 0x9, 0x9a, 0x20,

    /* U+0044 "D" */
    0x4b, 0x89, 0x24, 0x60, 0xa, 0x46, 0x0, 0xa4,
    0x60, 0xa, 0x4b, 0x89, 0x20,

    /* U+0045 "E" */
    0x4b, 0x99, 0x14, 0x60, 0x0, 0x4b, 0x98, 0x4,
    0x60, 0x0, 0x4b, 0x99, 0x10,

    /* U+0046 "F" */
    0x4b, 0x99, 0x4, 0x60, 0x0, 0x4b, 0x97, 0x4,
    0x60, 0x0, 0x46, 0x0, 0x0,

    /* U+0047 "G" */
    0x8, 0x9a, 0x55, 0x50, 0x2, 0x73, 0x8, 0xb5,
    0x50, 0xb, 0x9, 0x98, 0xc0,

    /* U+0048 "H" */
    0x46, 0x0, 0xa4, 0x60, 0xa, 0x4b, 0x99, 0xc4,
    0x60, 0xa, 0x46, 0x0, 0xa0,

    /* U+0049 "I" */
    0x4d, 0x60, 0xa0, 0xa, 0x0, 0xa0, 0x4d, 0x60,

    /* U+004A "J" */
    0x19, 0xb6, 0x0, 0x46, 0x0, 0x46, 0x20, 0x55,
    0x6a, 0xb1,

    /* U+004B "K" */
    0x46, 0x8, 0x44, 0x67, 0x50, 0x4c, 0xb1, 0x4,
    0x71, 0xa0, 0x46, 0x4, 0x70,

    /* U+004C "L" */
    0x46, 0x0, 0x46, 0x0, 0x46, 0x0, 0x46, 0x0,
    0x4b, 0x97,

    /* U+004D "M" */
    0x4d, 0x0, 0x6b, 0x49, 0x81, 0x9a, 0x46, 0x8a,
    0x1a, 0x46, 0x5, 0xa, 0x46, 0x0, 0xa,

    /* U+004E "N" */
    0x4d, 0x0, 0xa4, 0x98, 0xa, 0x46, 0x83, 0xa4,
    0x60, 0xaa, 0x46, 0x3, 0xe0,

    /* U+004F "O" */
    0x8, 0x99, 0x50, 0x55, 0x0, 0xa0, 0x73, 0x0,
    0x82, 0x55, 0x0, 0xa0, 0x8, 0x99, 0x50,

    /* U+0050 "P" */
    0x4b, 0x9a, 0x34, 0x60, 0x37, 0x4b, 0x99, 0x14,
    0x60, 0x0, 0x46, 0x0, 0x0,

    /* U+0051 "Q" */
    0x8, 0x9a, 0x40, 0x46, 0x0, 0xa0, 0x73, 0x0,
    0x82, 0x73, 0x0, 0x81, 0x29, 0x2, 0xa0, 0x3,
    0xb9, 0x10, 0x0, 0x39, 0x0,

    /* U+0052 "R" */
    0x4b, 0x9a, 0x34, 0x60, 0x37, 0x4b, 0x9b, 0x14,
    0x60, 0xb0, 0x46, 0x5, 0x60,

    /* U+0053 "S" */
    0x2a, 0x99, 0x6, 0x40, 0x0, 0x6, 0x87, 0x1,
    0x0, 0x82, 0x39, 0x89, 0x0,

    /* U+0054 "T" */
    0x79, 0xd9, 0x30, 0xa, 0x0, 0x0, 0xa0, 0x0,
    0xa, 0x0, 0x0, 0xa0, 0x0,

    /* U+0055 "U" */
    0x46, 0x0, 0xa4, 0x60, 0xa, 0x46, 0x0, 0xa3,
    0x70, 0xa, 0xa, 0x9a, 0x30,

    /* U+0056 "V" */
    0xa1, 0x2, 0x84, 0x60, 0x82, 0xb, 0xa, 0x0,
    0x85, 0x60, 0x2, 0xe0, 0x0,

    /* U+0057 "W" */
    0xa0, 0xe, 0x10, 0xa7, 0x34, 0x85, 0x18, 0x27,
    0x80, 0x96, 0x30, 0xa8, 0x9, 0x90, 0x9, 0x60,
    0x5a, 0x0,

    /* U+0058 "X" */
    0x66, 0x7, 0x40, 0xa5, 0x80, 0x2, 0xf1, 0x0,
    0xa4, 0x90, 0x74, 0x7, 0x50,

    /* U+0059 "Y" */
    0x83, 0x6, 0x50, 0xa1, 0x90, 0x4, 0xd1, 0x0,
    0xa, 0x0, 0x0, 0xa0, 0x0,

    /* U+005A "Z" */
    0x49, 0x9d, 0x20, 0x4, 0x70, 0x2, 0x90, 0x1,
    0xa0, 0x0, 0xaa, 0x99, 0x30,

    /* U+005B "[" */
    0x4a, 0x4, 0x40, 0x44, 0x4, 0x40, 0x44, 0x4,
    0x40, 0x28, 0x0,

    /* U+005C "\\" */
    0x81, 0x3, 0x50, 0x9, 0x0, 0x80, 0x4, 0x50,
    0x9, 0x0, 0x40,

    /* U+005D "]" */
    0x3a, 0x9, 0x9, 0x9, 0x9, 0x9, 0x38,

    /* U+005E "^" */
    0x3, 0xd1, 0x0, 0x91, 0x90, 0x53, 0x6, 0x20,
    0x0, 0x0,

    /* U+005F "_" */
    0x68, 0x88, 0x20,

    /* U+0060 "`" */
    0x34, 0x4,

    /* U+0061 "a" */
    0x28, 0x93, 0x2, 0x77, 0x80, 0x91, 0x19, 0x5,
    0x86, 0xa0,

    /* U+0062 "b" */
    0x55, 0x0, 0x5, 0x50, 0x0, 0x59, 0x89, 0x5,
    0x50, 0x82, 0x55, 0x8, 0x25, 0x98, 0x90,

    /* U+0063 "c" */
    0x29, 0x86, 0x81, 0x0, 0x81, 0x0, 0x29, 0x86,

    /* U+0064 "d" */
    0x0, 0xa, 0x0, 0xa, 0x2a, 0x7b, 0x82, 0xa,
    0x82, 0xa, 0x2a, 0x7b,

    /* U+0065 "e" */
    0x18, 0x87, 0x8, 0x87, 0xa0, 0x82, 0x2, 0x1,
    0xa9, 0x60,

    /* U+0066 "f" */
    0xa, 0x31, 0x80, 0x7c, 0x31, 0x80, 0x18, 0x1,
    0x80,

    /* U+0067 "g" */
    0x2a, 0x6a, 0x5, 0x40, 0x90, 0x1a, 0x83, 0x3,
    0xb9, 0x60, 0x80, 0x9, 0x15, 0x88, 0x90,

    /* U+0068 "h" */
    0x55, 0x0, 0x55, 0x0, 0x58, 0x88, 0x55, 0xa,
    0x55, 0xa, 0x55, 0xa,

    /* U+0069 "i" */
    0x33, 0x0, 0x55, 0x55, 0x55, 0x55,

    /* U+006A "j" */
    0x33, 0x0, 0x55, 0x55, 0x55, 0x55, 0x55, 0xa2,

    /* U+006B "k" */
    0x55, 0x0, 0x5, 0x50, 0x0, 0x55, 0x56, 0x5,
    0x99, 0x0, 0x57, 0xa1, 0x5, 0x51, 0xa0,

    /* U+006C "l" */
    0x55, 0x55, 0x55, 0x55, 0x55, 0x39,

    /* U+006D "m" */
    0x58, 0x79, 0x6a, 0x15, 0x50, 0xa0, 0x55, 0x55,
    0xa, 0x4, 0x55, 0x50, 0xa0, 0x45,

    /* U+006E "n" */
    0x58, 0x77, 0x55, 0xa, 0x55, 0xa, 0x55, 0xa,

    /* U+006F "o" */
    0x19, 0x86, 0x8, 0x10, 0x90, 0x81, 0x9, 0x1,
    0x98, 0x60,

    /* U+0070 "p" */
    0x58, 0x78, 0x5, 0x50, 0x72, 0x55, 0x8, 0x25,
    0x98, 0x90, 0x55, 0x0, 0x5, 0x50, 0x0,

    /* U+0071 "q" */
    0x2a, 0x7b, 0x82, 0xa, 0x82, 0xa, 0x2a, 0x7b,
    0x0, 0xa, 0x0, 0xa,

    /* U+0072 "r" */
    0x59, 0x75, 0x50, 0x55, 0x5, 0x50,

    /* U+0073 "s" */
    0x49, 0x91, 0x67, 0x20, 0x2, 0x85, 0x58, 0xa3,

    /* U+0074 "t" */
    0x8, 0x7, 0xc3, 0x18, 0x1, 0x80, 0xb, 0x30,

    /* U+0075 "u" */
    0x54, 0xa, 0x54, 0xa, 0x54, 0xa, 0x1b, 0x7b,

    /* U+0076 "v" */
    0x90, 0x9, 0x55, 0x63, 0x9, 0x90, 0x8, 0x80,

    /* U+0077 "w" */
    0x90, 0x58, 0x9, 0x54, 0x88, 0x17, 0x18, 0x87,
    0x73, 0xb, 0x42, 0xd0,

    /* U+0078 "x" */
    0x65, 0x37, 0x9, 0x90, 0x9, 0xa0, 0x74, 0x38,

    /* U+0079 "y" */
    0x90, 0x9, 0x54, 0x45, 0x9, 0x90, 0x9, 0x90,
    0x6, 0x40, 0x2a, 0x0,

    /* U+007A "z" */
    0x58, 0xc4, 0x3, 0x80, 0x19, 0x0, 0xa9, 0x83,

    /* U+007B "{" */
    0xa, 0x20, 0x80, 0x17, 0xa, 0x10, 0x17, 0x0,
    0x80, 0x8, 0x10,

    /* U+007C "|" */
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,

    /* U+007D "}" */
    0x38, 0x0, 0x90, 0x9, 0x0, 0x46, 0x9, 0x0,
    0x90, 0x36, 0x0,

    /* U+007E "~" */
    0x48, 0x30, 0x1, 0x5, 0x82
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 30, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 54, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 10, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 25, .adv_w = 77, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 43, .adv_w = 119, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 89, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 31, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 79, .adv_w = 43, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 90, .adv_w = 43, .box_w = 4, .box_h = 7, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 104, .adv_w = 58, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 110, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 35, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 122, .adv_w = 51, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 124, .adv_w = 35, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 49, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 136, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 149, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 162, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 227, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 266, .adv_w = 37, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 37, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 275, .adv_w = 77, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 290, .adv_w = 77, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 298, .adv_w = 77, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 313, .adv_w = 61, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 323, .adv_w = 114, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 344, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 86, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 409, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 89, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 435, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 51, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 489, .adv_w = 104, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 504, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 532, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 545, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 566, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 579, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 592, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 605, .adv_w = 87, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 618, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 631, .adv_w = 114, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 649, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 662, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 675, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 688, .adv_w = 41, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 699, .adv_w = 49, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 710, .adv_w = 41, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 717, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 727, .adv_w = 72, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 730, .adv_w = 77, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 732, .adv_w = 68, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 742, .adv_w = 74, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 757, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 765, .adv_w = 74, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 777, .adv_w = 70, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 787, .adv_w = 41, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 796, .adv_w = 68, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 811, .adv_w = 73, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 32, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 829, .adv_w = 32, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 837, .adv_w = 67, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 852, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 858, .adv_w = 112, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 872, .adv_w = 73, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 880, .adv_w = 72, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 890, .adv_w = 74, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 905, .adv_w = 74, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 917, .adv_w = 47, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 923, .adv_w = 62, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 931, .adv_w = 45, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 939, .adv_w = 73, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 947, .adv_w = 63, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 955, .adv_w = 98, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 967, .adv_w = 65, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 975, .adv_w = 64, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 987, .adv_w = 59, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 995, .adv_w = 44, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1006, .adv_w = 40, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1013, .adv_w = 44, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1024, .adv_w = 77, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2}
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
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 3, 4, 5, 6, 7, 8,
    9, 0, 10, 11, 12, 13, 0, 0,
    14, 15, 14, 16, 17, 18, 19, 20,
    21, 22, 18, 23, 0, 0, 0, 0,
    0, 0, 24, 25, 26, 0, 27, 28,
    29, 30, 31, 32, 33, 34, 30, 30,
    35, 25, 36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 0, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 0, 0, 3,
    2, 0, 4, 2, 0, 5, 6, 5,
    7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 8, 8, 0, 0, 0,
    9, 10, 11, 0, 12, 0, 0, 0,
    12, 0, 13, 14, 0, 0, 0, 0,
    12, 0, 12, 0, 15, 16, 17, 18,
    19, 20, 21, 22, 0, 0, 23, 0,
    24, 0, 25, 0, 26, 27, 26, 28,
    29, 0, 0, 30, 0, 31, 32, 32,
    26, 33, 27, 32, 34, 35, 36, 37,
    38, 39, 40, 41, 0, 0, 23, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 1, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -1, 0, 2, -3,
    0, 0, -1, -1, 2, -2, 0, 1,
    0, -8, -2, -5, -1, 1, -7, 0,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, -3, -1,
    0, -2, 1, 0, 0, 0, 0, -2,
    0, -2, 0, 0, 0, -1, 0, 0,
    -1, 0, -3, 0, -3, -1, -3, -4,
    0, 0, -8, 0, 0, 0, -1, -1,
    0, 0, 0, 0, 0, -1, 0, -1,
    -1, -1, -1, 0, 0, 1, -1, 0,
    0, -3, 0, 0, 0, -1, 0, -2,
    0, 0, 0, 0, 0, 0, 1, 0,
    -1, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, -4, 2, -1, 0, 0, 0, -2,
    0, 0, -2, 0, -3, 0, -2, 0,
    -2, -4, -2, 0, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, -1,
    0, 0, 0, -2, 0, 0, 0, -1,
    0, 0, -3, 0, 0, 0, 0, -1,
    -1, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, -4, 2, -10, 0, -5, 0, 2,
    -3, -6, -2, 0, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -17, -5,
    -3, -3, -1, -5, -1, 0, -3, -3,
    -3, -1, -3, -3, -1, -4, -3, -1,
    0, -1, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, -2, 0, -1, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, -4, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, -1, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -1, 0, 0, 0, -1, 0, 0, -1,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, -3, 1, 1,
    -7, 0, 0, 0, -5, 0, -3, 0,
    -2, -1, -3, 0, -2, -1, 0, -2,
    0, 0, 0, -2, -3, -3, -3, -1,
    0, 0, -1, -1, 0, -3, -1, -1,
    -1, -1, -1, 1, 0, -9, -1, 0,
    3, -5, 3, 0, -6, -3, 2, -4,
    0, 0, 0, -9, -4, -8, -3, 0,
    -9, 0, 0, 4, 0, -1, -1, 0,
    0, 0, 0, 0, 0, 0, -1, -1,
    -6, -4, 1, -5, 2, 0, -1, 0,
    0, -4, 2, -1, 0, 0, 0, -2,
    0, 0, -2, 0, -3, 0, -2, 0,
    -2, -4, -2, 0, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
    -4, 0, -13, 0, -6, 0, 0, -1,
    -6, 0, 0, -5, 0, 0, 0, 0,
    1, -2, 0, -3, 0, -17, -1, -3,
    -3, 0, -4, 0, 0, -1, -1, -1,
    0, 0, 0, 1, -2, 0, 0, 0,
    0, -3, 0, 0, -2, -1, 0, 0,
    -3, 0, -1, 0, -3, 0, -3, -2,
    -3, -1, 0, -5, 0, 0, 0, -1,
    -2, -2, 0, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 1, -1, 0,
    0, 0, 0, 0, 0, 0, -1, -2,
    0, 0, 0, 0, -1, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -3, 0, 0, 0, -2, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, -1, 1,
    -4, -1, -1, 0, 4, -3, -1, 0,
    0, -3, 0, 2, 0, 1, 1, 0,
    1, 0, 0, -6, -1, -2, -2, 0,
    -3, 0, 0, -1, -1, -1, 0, -1,
    0, -1, -1, 0, -1, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
    -1, 1, 4, -5, 0, 0, 0, -3,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, 0, 0, 0, -1, 0, 0,
    -2, 0, 0, 1, -1, 3, 0, 0,
    0, 1, 0, 0, 0, 0, -9, 0,
    -4, 0, 0, -6, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, -1, 0,
    0, -3, 0, 0, -3, 1, 0, 0,
    0, 0, 0, 0, -1, 0, 0, -9,
    0, -4, -2, -2, -5, 0, 0, -8,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, 0, -1, 0, -1, -1, -2, -1,
    -1, 0, 0, -1, 0, 0, 0, 1,
    0, 0, 0, 1, 0, 0, 1, 0,
    -6, 0, -3, -1, 0, -4, 1, 0,
    -1, 0, -1, -1, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, -3, 1,
    0, 0, 0, 0, 0, 0, -1, 0,
    -1, -9, 0, -4, -2, -2, -5, -1,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, -1,
    -2, -1, -1, 3, 4, -1, 3, -4,
    0, -3, 0, 4, 0, -1, 2, 0,
    -3, 0, 4, 0, 2, 3, 1, 2,
    0, 2, -9, 0, -1, 0, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 3,
    2, 0, 3, 0, 0, -1, 2, 1,
    0, -1, 0, -2, 0, -1, 0, 1,
    -1, 1, 1, 6, 0, -1, -1, 0,
    0, 1, 0, 0, 0, 0, 0, -1,
    0, 1, 0, 1, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -7, 0, -4, -3,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    -3, -1, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, -6, 0, 0,
    0, -3, 2, -1, 0, -1, 0, -4,
    0, -2, -1, 0, -6, 2, 0, 1,
    -1, -2, -2, 0, -1, 0, 0, 0,
    0, -1, -1, -1, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 1, -1, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    -3, 0, -1, 0, 2, -3, 2, 0,
    2, 0, 0, 0, 0, 0, 0, -1,
    0, 0, 0, -1, 0, -1, -1, 0,
    -1, 0, 0, -3, 0, 0, -3, 1,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, -9, 0, -4, -2, -2, -6, -1,
    0, -6, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, -1, -1,
    -2, -1, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, -3, -1, 0, -8,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, -3, 0,
    -10, -3, -2, 0, 0, 0, -3, 1,
    0, -3, 1, -1, 0, 0, 2, -3,
    -3, -1, 0, -10, -1, -1, -1, 1,
    -3, 0, 0, 0, 0, 0, 1, 0,
    1, 1, 0, 1, 0, 0, -1, -1,
    0, -1, -1, 0, 0, 0, 0, -1,
    0, 0, -1, 0, -8, 0, -4, 0,
    -3, -5, 0, 0, -3, 0, 0, 0,
    -1, -1, 0, 0, 0, 0, -1, -1,
    0, -2, -1, -3, -2, -1, 0, 0,
    -2, 0, 0, -3, 0, 0, 0, -1,
    1, 0, 0, 0, 0, -3, 0, 0,
    0, 0, -3, 0, 3, 1, 0, -1,
    -1, -1, -1, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -6, 0,
    -3, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, -1, 0, -6, -1, -1, 0,
    0, 0, -3, 0, 0, 0, 0, -4,
    0, -1, 0, 0, -3, 0, 0, -9,
    -1, -1, -1, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, -3, 0, -4, -1, 0,
    0, 0, 0, -1, 0, 0, 0, 0,
    -5, 0, -1, -1, 0, -3, 0, 0,
    -5, -1, -1, -1, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, -3, 0, 0, -4,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, -6, 0, -1, -1, 0, -3, 0,
    0, 3, 0, -2, -2, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 3, -3, 0, -7,
    -1, -1, 0, 0, -1, -2, 0, 0,
    -3, 0, -5, 0, -1, 0, 0, -3,
    0, 0, -10, -2, -1, -1, 0, -4,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, -1, 0,
    2, -3, 0, 0, 0, -1, 1, 0,
    0, 0, 0, -5, 0, -1, -1, 1,
    -3, 0, 0, 1, 0, -1, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 45,
    .right_class_cnt     = 41,
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
const lv_font_t tinyos_plex_8 = {
#else
lv_font_t tinyos_plex_8 = {
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



#endif /*#if TINYOS_PLEX_8*/

