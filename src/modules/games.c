#include "module.h"

#include "../core/log.h"
#include "../emulator/emulator.h"
#include "../emulator/rom.h"
#include "../ui/layout.h"
#include "../ui/scale.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    tinyos_ctx *ctx;
    tinyos_rom_entry roms[TINYOS_ROM_MAX];
    int count;
} games_view;

static char games_notice[128];

static void back_cb(lv_event_t *event)
{
    (void)event;
    tinyos_shell_back(tinyos_ctx_get());
}

static void rescan_cb(lv_event_t *event)
{
    tinyos_ctx *ctx = lv_event_get_user_data(event);
    tinyos_shell_reload(ctx, "games");
}

static void start_game(games_view *view, int index)
{
    if (!view || index < 0 || index >= view->count)
        return;

    tinyos_ctx *ctx = view->ctx;
    tinyos_rom_entry *rom = &view->roms[index];
    int result = tinyos_emulator_run(rom);

    if (result != 0) {
        snprintf(games_notice, sizeof(games_notice), "%s",
                 tinyos_emulator_last_error());
        TINYOS_LOGW("game failed: %s", games_notice);
    } else {
        games_notice[0] = '\0';
    }

    /* The emulator painted the panel directly. Rebuild the shell before the
     * normal LVGL loop resumes, then force one full refresh so no game pixel
     * survives behind a partial refresh. Capture ctx first: rebuilding
     * deletes the old screen and its view. */
    tinyos_shell_reload(ctx, "games");
    if (ctx->display) {
        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(ctx->display);
    }
}

static void game_click_cb(lv_event_t *event)
{
    games_view *view = lv_event_get_user_data(event);
    int index = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_target(event));
    start_game(view, index);
}

static lv_obj_t *rom_row(games_view *view, lv_obj_t *list, int index)
{
    bool large = tinyos_large_text();
    tinyos_rom_entry *rom = &view->roms[index];
    int height = large ? 36 : 32;

    lv_obj_t *row = tinyos_obj(list, 0, 0, TINYOS_CONTENT_W, height);
    lv_obj_add_style(row, &tinyos_theme.row, 0);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_style_radius(row, TINYOS_ROW_RADIUS, 0);
    tinyos_focus(row);
    lv_obj_set_user_data(row, (void *)(intptr_t)index);
    tinyos_add_click(row, game_click_cb, view);

    int badge_w = large ? 38 : 28;
    int badge_h = large ? 28 : 19;
    lv_obj_t *badge = tinyos_obj(row, large ? 6 : 8, (height - badge_h) / 2,
                                 badge_w, badge_h);
    lv_obj_add_style(badge, &tinyos_theme.pill, 0);
    lv_obj_set_style_bg_color(badge, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
    lv_obj_t *tag = tinyos_label(badge, tinyos_rom_system_tag(rom->system),
                                 &lv_font_montserrat_10, TINYOS_COLOR_CYAN, 0, 0,
                                 badge_w, badge_h);
    lv_obj_set_style_text_align(tag, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *name = tinyos_label(row, rom->name, &lv_font_montserrat_12,
                                  TINYOS_COLOR_TEXT, large ? 48 : 42,
                                  large ? 6 : 2, large ? 140 : 154,
                                  large ? 24 : 16);
    lv_label_set_long_mode(name, LV_LABEL_LONG_MODE_DOTS);
    if (!large) {
        tinyos_label(row, tinyos_rom_system_name(rom->system), &lv_font_montserrat_8,
                     TINYOS_COLOR_MUTED, 42, 18, 154, 10);
    }
    return row;
}

static void games_delete_cb(lv_event_t *event)
{
    games_view *view = lv_event_get_user_data(event);
    free(view);
}

static lv_obj_t *games_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    games_view *view = calloc(1, sizeof(*view));
    if (!view)
        return parent;
    view->ctx = ctx;
    view->count = tinyos_roms_scan(view->roms, TINYOS_ROM_MAX);
    lv_obj_add_event_cb(parent, games_delete_cb, LV_EVENT_DELETE, view);

    bool large = tinyos_large_text();
    lv_obj_t *back = tinyos_back_button(parent, back_cb);
    tinyos_page_title(parent, "Games", 0);

    if (view->count == 0) {
        tinyos_icon(parent, LV_SYMBOL_PLAY, TINYOS_COLOR_CYAN,
                    TINYOS_CONTENT_X + 92, 42, 28);
        tinyos_label(parent, "No games found", &lv_font_montserrat_16,
                     TINYOS_COLOR_TEXT, TINYOS_CONTENT_X, 78, TINYOS_CONTENT_W, 22);
        lv_obj_t *hint = tinyos_label(parent,
                                      "Copy .gb/.gbc/.gba ROMs to\n"
                                      TINYOS_ROMS_ROOT "/gb, /gbc or /gba.",
                                      &lv_font_montserrat_10, TINYOS_COLOR_MUTED,
                                      TINYOS_CONTENT_X, 102, TINYOS_CONTENT_W, 48);
        lv_label_set_long_mode(hint, LV_LABEL_LONG_MODE_WRAP);

        lv_obj_t *rescan = tinyos_obj(parent, TINYOS_CONTENT_X, 150, 112, 24);
        lv_obj_add_style(rescan, &tinyos_theme.pill, 0);
        lv_obj_set_style_bg_color(rescan, lv_color_hex(TINYOS_COLOR_SURFACE_2), 0);
        tinyos_focus(rescan);
        tinyos_add_click(rescan, rescan_cb, ctx);
        tinyos_label(rescan, "SCAN AGAIN", &lv_font_montserrat_10, TINYOS_COLOR_TEXT,
                     0, 5, 112, 14);
        lv_obj_t *rescan_label = lv_obj_get_child(rescan, 0);
        if (rescan_label)
            lv_obj_set_style_text_align(rescan_label, LV_TEXT_ALIGN_CENTER, 0);
        tinyos_shell_add_focus(ctx, rescan);
        tinyos_shell_add_focus(ctx, back);
        return parent;
    }

    lv_obj_t *summary = tinyos_label(parent, "", &lv_font_montserrat_8,
                                     TINYOS_COLOR_MUTED, TINYOS_CONTENT_X, 27,
                                     TINYOS_CONTENT_W, 10);
    char summary_text[32];
    snprintf(summary_text, sizeof(summary_text), "%d GAME%s", view->count,
             view->count == 1 ? "" : "S");
    lv_label_set_text(summary, summary_text);
    lv_obj_set_style_text_letter_space(summary, 1, 0);

    /* Keep the viewport at a row boundary. Otherwise the next row is
     * partially visible below the list and its labels escape the list box;
     * the layout audit correctly rejects that clipping. */
    int list_height = large ? 112 : 104;
    lv_obj_t *list = tinyos_obj(parent, TINYOS_LIST_X, 38, TINYOS_LIST_W, list_height);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, TINYOS_ROW_GAP, 0);
    lv_obj_set_style_pad_all(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_left(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_style_pad_right(list, TINYOS_LIST_PAD, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, large ? LV_SCROLLBAR_MODE_ON : LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_width(list, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(list, lv_color_hex(TINYOS_COLOR_MUTED), LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(list, LV_OPA_40, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(list, 2, LV_PART_SCROLLBAR);
    lv_obj_set_scrollable(list, true);

    for (int i = 0; i < view->count; i++) {
        lv_obj_t *row = rom_row(view, list, i);
        tinyos_shell_add_focus(ctx, row);
    }

    if (games_notice[0]) {
        lv_obj_t *notice = tinyos_label(parent, games_notice, &lv_font_montserrat_8,
                                        TINYOS_COLOR_RED, TINYOS_CONTENT_X, 158,
                                        TINYOS_CONTENT_W, 12);
        lv_label_set_long_mode(notice, LV_LABEL_LONG_MODE_DOTS);
        games_notice[0] = '\0';
    } else {
        tinyos_label(parent, "Hold X + Y to leave a game", &lv_font_montserrat_8,
                     TINYOS_COLOR_DIM, TINYOS_CONTENT_X, 158, TINYOS_CONTENT_W, 12);
    }

    /* The list is the primary target, so add the back button last. It remains
     * reachable with Up/Left and the shell's B/back path. */
    tinyos_shell_add_focus(ctx, back);
    return parent;
}

const tinyos_module tinyos_module_games = {
    .id = "games",
    .name = "Games",
    .subtitle = "Game Boy, Color and Advance",
    .icon = LV_SYMBOL_PLAY,
    .create = games_create,
};
