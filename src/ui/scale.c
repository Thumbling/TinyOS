#include "scale.h"

#include "../app.h"
#include "fonts.h"

#include <stddef.h>

static const int ramp_115[TINYOS_FONT_SIZE_COUNT] = { 10, 12, 14, 16, 20, 24, 24, 28 };
static const int ramp_130[TINYOS_FONT_SIZE_COUNT] = { 10, 14, 16, 20, 20, 24, 24, 28 };

int tinyos_text_scale(void)
{
    tinyos_ctx *ctx = tinyos_ctx_get();
    if (!ctx)
        return 100;
    if (ctx->settings.text_scale >= 130)
        return 130;
    if (ctx->settings.text_scale >= 115)
        return 115;
    return 100;
}

bool tinyos_large_text(void)
{
    return tinyos_text_scale() > 100;
}

int tinyos_text_px(int base)
{
    int scale = tinyos_text_scale();
    int value = (base * scale + 50) / 100;
    return value < 1 ? 1 : value;
}

const lv_font_t *tinyos_font_scale(const lv_font_t *font)
{
    static const lv_font_t *const montserrat[TINYOS_FONT_SIZE_COUNT] = {
        &lv_font_montserrat_8,  &lv_font_montserrat_10, &lv_font_montserrat_12,
        &lv_font_montserrat_14, &lv_font_montserrat_16, &lv_font_montserrat_20,
        &lv_font_montserrat_24, &lv_font_montserrat_28,
    };

    int index = -1;
    for (int i = 0; i < TINYOS_FONT_SIZE_COUNT; i++) {
        if (font == montserrat[i]) {
            index = i;
            break;
        }
    }
    if (index < 0)
        return font;
    if (tinyos_text_scale() <= 100)
        return tinyos_font_map(font);

    int target = tinyos_text_scale() >= 130 ? ramp_130[index] : ramp_115[index];
    return tinyos_font_family(tinyos_fonts_active(), target);
}
