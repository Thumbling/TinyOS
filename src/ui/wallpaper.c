#include "ui/wallpaper.h"

#include "ui/theme.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WP_MAX_ANIMS 64

typedef struct {
    void *var;
    lv_anim_exec_xcb_t callback;
} wp_animation;

struct tinyos_wallpaper {
    lv_obj_t *root;
    int width;
    int height;
    int style;
    int speed;
    bool preview;
    wp_animation animations[WP_MAX_ANIMS];
    int animation_count;
};

static const char *const style_names[TINYOS_WALLPAPER_COUNT] = {
    "Aurora", "Nebula", "Orbit", "Waves", "Grid",
    "Stars", "Prism", "Ripple", "Mesh", "Minimal",
};

static const char *const speed_names[TINYOS_WALLPAPER_SPEEDS] = {
    "Still", "Slow", "Normal", "Fast",
};

static void animate_translate_x(void *var, int32_t value)
{
    lv_obj_set_style_translate_x((lv_obj_t *)var, value, 0);
}

static void animate_translate_y(void *var, int32_t value)
{
    lv_obj_set_style_translate_y((lv_obj_t *)var, value, 0);
}

static void animate_opa(void *var, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)value, 0);
}

static void animate_scale(void *var, int32_t value)
{
    lv_obj_set_style_transform_scale((lv_obj_t *)var, value, 0);
}

static void remember_animation(tinyos_wallpaper *wallpaper, void *var,
                               lv_anim_exec_xcb_t callback)
{
    if (wallpaper->animation_count >= WP_MAX_ANIMS)
        return;
    wallpaper->animations[wallpaper->animation_count].var = var;
    wallpaper->animations[wallpaper->animation_count].callback = callback;
    wallpaper->animation_count++;
}

static uint32_t scaled(tinyos_wallpaper *wallpaper, uint32_t duration)
{
    if (wallpaper->speed <= 0)
        return 0;
    static const uint16_t factor[TINYOS_WALLPAPER_SPEEDS] = {0, 185, 100, 58};
    return (duration * factor[wallpaper->speed]) / 100u;
}

static void pingpong(tinyos_wallpaper *wallpaper, lv_obj_t *object,
                     lv_anim_exec_xcb_t callback, int32_t from, int32_t to,
                     uint32_t duration, uint32_t delay)
{
    uint32_t value_duration = scaled(wallpaper, duration);
    if (value_duration < 300)
        return;

    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, object);
    lv_anim_set_values(&animation, from, to);
    lv_anim_set_duration(&animation, value_duration);
    lv_anim_set_reverse_duration(&animation, value_duration);
    lv_anim_set_repeat_count(&animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&animation, 120);
    lv_anim_set_delay(&animation, delay);
    lv_anim_set_exec_cb(&animation, callback);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
    if (lv_anim_start(&animation)) {
        remember_animation(wallpaper, object, callback);
    }
}

static lv_obj_t *wp_shape(tinyos_wallpaper *wallpaper, int x, int y,
                          int width, int height, uint32_t color, lv_opa_t opa,
                          int radius)
{
    lv_obj_t *object = lv_obj_create(wallpaper->root);
    lv_obj_remove_style_all(object);
    lv_obj_set_pos(object, x, y);
    lv_obj_set_size(object, width, height);
    lv_obj_set_scrollable(object, false);
    lv_obj_set_style_radius(object, radius, 0);
    lv_obj_set_style_bg_color(object, lv_color_hex(color), 0);
    if (wallpaper->preview && opa > 0 && opa < 180) {
        int boosted = (int)opa + 18;
        opa = (lv_opa_t)(boosted > 200 ? 200 : boosted);
    }
    lv_obj_set_style_bg_opa(object, opa, 0);
    lv_obj_set_style_border_width(object, 0, 0);
    lv_obj_set_style_pad_all(object, 0, 0);
    return object;
}

static lv_obj_t *wp_ring(tinyos_wallpaper *wallpaper, int x, int y,
                         int size, uint32_t color, lv_opa_t opa, int border)
{
    lv_obj_t *object = wp_shape(wallpaper, x, y, size, size, color, LV_OPA_TRANSP,
                                LV_RADIUS_CIRCLE);
    lv_obj_set_style_border_width(object, border, 0);
    lv_obj_set_style_border_color(object, lv_color_hex(color), 0);
    lv_obj_set_style_border_opa(object, opa, 0);
    return object;
}

static void wp_blob(tinyos_wallpaper *wallpaper, int x, int y, int size,
                    uint32_t color, lv_opa_t opa)
{
    wp_shape(wallpaper, x, y, size, size, color, opa, LV_RADIUS_CIRCLE);
}

static void build_aurora(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    lv_obj_t *a = wp_shape(wallpaper, -w / 4, -h / 3, w * 3 / 4, h * 3 / 5,
                           TINYOS_COLOR_CYAN, 18, LV_RADIUS_CIRCLE);
    lv_obj_t *b = wp_shape(wallpaper, w / 3, -h / 5, w * 2 / 3, h / 2,
                           TINYOS_COLOR_VIOLET, 14, LV_RADIUS_CIRCLE);
    lv_obj_t *c = wp_shape(wallpaper, w / 6, h * 2 / 3, w / 2, h / 2,
                           TINYOS_COLOR_GREEN, 10, LV_RADIUS_CIRCLE);
    pingpong(wallpaper, a, animate_translate_x, 0, w / 12, 15000, 0);
    pingpong(wallpaper, b, animate_translate_x, 0, -w / 14, 18000, 700);
    pingpong(wallpaper, c, animate_translate_y, 0, -h / 12, 21000, 1200);
    lv_obj_t *ring = wp_ring(wallpaper, w * 3 / 5, h / 4, w / 3,
                             TINYOS_COLOR_CYAN, 26, 1);
    pingpong(wallpaper, ring, animate_opa, 12, 38, 9000, 300);
}

static void build_nebula(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    uint32_t colors[4] = {TINYOS_COLOR_CYAN, TINYOS_COLOR_VIOLET,
                          TINYOS_COLOR_GREEN, TINYOS_COLOR_CYAN};
    for (int i = 0; i < 4; i++) {
        lv_obj_t *blob = wp_shape(wallpaper, -w / 4 + i * w / 4, -h / 4 + (i % 2) * h / 3,
                                  w * 2 / 3, h * 2 / 3, colors[i], 15,
                                  LV_RADIUS_CIRCLE);
        pingpong(wallpaper, blob, animate_translate_x, 0, (i % 2 ? -1 : 1) * w / 10,
                 13000 + i * 1800, i * 400);
        if (!wallpaper->preview) {
            pingpong(wallpaper, blob, animate_translate_y, 0, (i % 2 ? 1 : -1) * h / 14,
                     17000 + i * 1300, i * 260);
        }
    }
    wp_ring(wallpaper, w * 2 / 5, h * 2 / 5, w / 4, TINYOS_COLOR_VIOLET, 22, 1);
}

static void build_orbit(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    int size = w / 2;
    wp_ring(wallpaper, w / 2 - size / 2, h / 2 - size / 2, size,
            TINYOS_COLOR_CYAN, 26, 1);
    wp_ring(wallpaper, w / 2 - size / 4, h / 2 - size / 4, size / 2,
            TINYOS_COLOR_VIOLET, 22, 1);

    for (int i = 0; i < 3; i++) {
        int dot_size = wallpaper->preview ? 3 : 5;
        lv_obj_t *dot = wp_shape(wallpaper, w / 2 + (i - 1) * w / 4, h / 2 - h / 6,
                                 dot_size, dot_size, TINYOS_COLOR_CYAN, 190,
                                 LV_RADIUS_CIRCLE);
        pingpong(wallpaper, dot, animate_translate_x, -w / 14, w / 14,
                 11000 + i * 2200, i * 600);
        pingpong(wallpaper, dot, animate_translate_y, -h / 20, h / 20,
                 15000 + i * 1800, i * 350);
    }
}

static void build_waves(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    for (int i = 0; i < 3; i++) {
        lv_obj_t *band = wp_shape(wallpaper, -w / 5 + i * w / 12, h / 5 + i * h / 4,
                                  w * 3 / 2, h / 10,
                                  i % 2 ? TINYOS_COLOR_VIOLET : TINYOS_COLOR_CYAN,
                                  i == 1 ? 12 : 18, h / 16);
        pingpong(wallpaper, band, animate_translate_x, -w / 10, w / 10,
                 16000 + i * 2400, i * 500);
        pingpong(wallpaper, band, animate_translate_y, -h / 24, h / 24,
                 20000 + i * 1500, i * 300);
    }
}

static void build_grid(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    lv_obj_t *grid = wp_shape(wallpaper, 0, 0, w, h, TINYOS_COLOR_LINE, 0, 0);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_clip_corner(grid, true, 0);
    for (int i = 1; i < 6; i++) {
        lv_obj_t *vertical = wp_shape(wallpaper, i * w / 6, 0, 1, h,
                                      TINYOS_COLOR_LINE, 70, 0);
        lv_obj_set_parent(vertical, grid);
    }
    for (int i = 1; i < 5; i++) {
        lv_obj_t *horizontal = wp_shape(wallpaper, 0, i * h / 5, w, 1,
                                        TINYOS_COLOR_LINE, 70, 0);
        lv_obj_set_parent(horizontal, grid);
    }
    lv_obj_t *glow = wp_shape(wallpaper, w / 5, h / 5, w / 2, h / 2,
                              TINYOS_COLOR_CYAN, 10, LV_RADIUS_CIRCLE);
    lv_obj_set_parent(glow, grid);
    pingpong(wallpaper, grid, animate_translate_x, -4, 4, 17000, 200);
    pingpong(wallpaper, grid, animate_translate_y, -3, 3, 13000, 600);
    pingpong(wallpaper, glow, animate_opa, 8, 22, 11000, 0);
}

static void build_stars(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    int count = wallpaper->preview ? 6 : 11;
    for (int i = 0; i < count; i++) {
        int x = (i * 71 + 13) % (w - 4);
        int y = (i * 47 + 9) % (h - 4);
        int size = (i % 3 == 0) ? 2 : 1;
        lv_obj_t *star = wp_shape(wallpaper, x, y, size, size,
                                  i % 4 ? TINYOS_COLOR_TEXT : TINYOS_COLOR_CYAN,
                                  90, LV_RADIUS_CIRCLE);
        pingpong(wallpaper, star, animate_opa, 24, 150, 5000 + (i % 5) * 1200,
                 i * 350);
        if (!wallpaper->preview && i % 2 == 0) {
            pingpong(wallpaper, star, animate_translate_y, 0, -h / 20,
                     15000 + i * 400, i * 230);
        }
    }
    wp_blob(wallpaper, -w / 5, h * 2 / 3, w / 2, TINYOS_COLOR_VIOLET, 9);
    wp_blob(wallpaper, w * 3 / 5, -h / 5, w / 2, TINYOS_COLOR_CYAN, 9);
}

static void build_prism(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    for (int i = 0; i < 4; i++) {
        lv_obj_t *band = wp_shape(wallpaper, -w / 5, h / 5 + i * h / 5,
                                  w * 3 / 2, h / 7,
                                  i % 2 ? TINYOS_COLOR_VIOLET : TINYOS_COLOR_CYAN,
                                  i == 2 ? 10 : 16, h / 16);
        pingpong(wallpaper, band, animate_translate_x, -w / 8, w / 8,
                 13000 + i * 2600, i * 450);
    }
}

static void build_ripple(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    int size = w / 4;
    for (int i = 0; i < 3; i++) {
        lv_obj_t *ring = wp_ring(wallpaper, w / 2 - size / 2, h / 2 - size / 2,
                                 size, i % 2 ? TINYOS_COLOR_VIOLET : TINYOS_COLOR_CYAN,
                                 30 - i * 5, 1);
        lv_obj_set_style_transform_pivot_x(ring, size / 2, 0);
        lv_obj_set_style_transform_pivot_y(ring, size / 2, 0);
        pingpong(wallpaper, ring, animate_scale, 256, 420 + i * 50,
                 8000 + i * 2200, i * 650);
        pingpong(wallpaper, ring, animate_opa, 10, 55 - i * 10,
                 8000 + i * 2200, i * 650);
    }
}

static void build_mesh(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    wp_blob(wallpaper, -w / 4, h / 4, w * 2 / 3, TINYOS_COLOR_CYAN, 11);
    wp_blob(wallpaper, w / 2, h / 2, w * 2 / 3, TINYOS_COLOR_VIOLET, 11);
    for (int i = 0; i < 4; i++) {
        int x1 = (i * w) / 4;
        int y1 = (i % 2) * h;
        lv_obj_t *line = wp_shape(wallpaper, x1, y1 - h / 2, 1, h * 2,
                                  TINYOS_COLOR_LINE, 50, 0);
        pingpong(wallpaper, line, animate_translate_x, -w / 16, w / 16,
                 19000 + i * 1500, i * 420);
    }
    lv_obj_t *ring = wp_ring(wallpaper, w / 3, h / 3, w / 4,
                             TINYOS_COLOR_CYAN, 20, 1);
    lv_obj_set_style_transform_pivot_x(ring, w / 8, 0);
    lv_obj_set_style_transform_pivot_y(ring, w / 8, 0);
    pingpong(wallpaper, ring, animate_scale, 256, 340, 14000, 300);
}

static void build_minimal(tinyos_wallpaper *wallpaper)
{
    int w = wallpaper->width, h = wallpaper->height;
    wp_blob(wallpaper, w * 2 / 3, -h / 8, w / 2, TINYOS_COLOR_CYAN, 9);
    wp_ring(wallpaper, w / 2 - w / 6, h / 2 - w / 6, w / 3,
            TINYOS_COLOR_CYAN, 22, 1);
    wp_shape(wallpaper, w / 8, h * 2 / 3, w / 20, 1, TINYOS_COLOR_VIOLET, 80, 0);
}

static void wallpaper_delete_cb(lv_event_t *event)
{
    tinyos_wallpaper *wallpaper = lv_event_get_user_data(event);
    if (!wallpaper)
        return;
    for (int i = 0; i < wallpaper->animation_count; i++) {
        lv_anim_delete(wallpaper->animations[i].var,
                       wallpaper->animations[i].callback);
    }
    free(wallpaper);
}

tinyos_wallpaper *tinyos_wallpaper_create(lv_obj_t *parent, int width, int height,
                                          int style, int speed, bool preview)
{
    if (!parent || width <= 0 || height <= 0)
        return NULL;

    tinyos_wallpaper *wallpaper = calloc(1, sizeof(*wallpaper));
    if (!wallpaper)
        return NULL;

    wallpaper->width = width;
    wallpaper->height = height;
    wallpaper->style = tinyos_wallpaper_clamp_style(style);
    wallpaper->speed = tinyos_wallpaper_clamp_speed(speed);
    wallpaper->preview = preview;

    wallpaper->root = lv_obj_create(parent);
    lv_obj_remove_style_all(wallpaper->root);
    lv_obj_set_pos(wallpaper->root, 0, 0);
    lv_obj_set_size(wallpaper->root, width, height);
    lv_obj_set_scrollable(wallpaper->root, false);
    lv_obj_set_style_radius(wallpaper->root, 0, 0);
    lv_obj_set_style_clip_corner(wallpaper->root, true, 0);
    lv_obj_set_style_bg_color(wallpaper->root, lv_color_hex(TINYOS_COLOR_BG_2), 0);
    lv_obj_set_style_bg_grad_color(wallpaper->root, lv_color_hex(TINYOS_COLOR_BG), 0);
    lv_obj_set_style_bg_grad_dir(wallpaper->root, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(wallpaper->root, LV_OPA_COVER, 0);
    lv_obj_add_event_cb(wallpaper->root, wallpaper_delete_cb, LV_EVENT_DELETE, wallpaper);

    switch (wallpaper->style) {
    case 1:
        build_nebula(wallpaper);
        break;
    case 2:
        build_orbit(wallpaper);
        break;
    case 3:
        build_waves(wallpaper);
        break;
    case 4:
        build_grid(wallpaper);
        break;
    case 5:
        build_stars(wallpaper);
        break;
    case 6:
        build_prism(wallpaper);
        break;
    case 7:
        build_ripple(wallpaper);
        break;
    case 8:
        build_mesh(wallpaper);
        break;
    case 9:
        build_minimal(wallpaper);
        break;
    default:
        build_aurora(wallpaper);
        break;
    }
    return wallpaper;
}

void tinyos_wallpaper_destroy(tinyos_wallpaper *wallpaper)
{
    if (wallpaper && wallpaper->root) {
        lv_obj_delete(wallpaper->root);
    }
}

int tinyos_wallpaper_clamp_style(int style)
{
    if (style < 0 || style >= TINYOS_WALLPAPER_COUNT)
        return 0;
    return style;
}

int tinyos_wallpaper_clamp_speed(int speed)
{
    if (speed < 0 || speed >= TINYOS_WALLPAPER_SPEEDS)
        return 2;
    return speed;
}

const char *tinyos_wallpaper_name(int style)
{
    return style_names[tinyos_wallpaper_clamp_style(style)];
}

const char *tinyos_wallpaper_speed_name(int speed)
{
    return speed_names[tinyos_wallpaper_clamp_speed(speed)];
}
