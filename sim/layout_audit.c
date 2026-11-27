#include "layout_audit.h"

#include <lvgl/lvgl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../src/ui/layout.h"

#define PANEL_WIDTH 240
#define PANEL_HEIGHT 240
#define MAX_REPORTED_FAILURES 80

static const lv_obj_t *audit_screen;
static const lv_obj_t *audit_content;
static const char *audit_name;
static int failures;
static int labels_checked;
static int objects_checked;
static int reported_failures;

static bool area_intersects(const lv_area_t *a, const lv_area_t *b)
{
    return a->x1 <= b->x2 && a->x2 >= b->x1 && a->y1 <= b->y2 && a->y2 >= b->y1;
}

static bool area_inside(const lv_area_t *inner, const lv_area_t *outer)
{
    return inner->x1 >= outer->x1 && inner->x2 <= outer->x2 &&
           inner->y1 >= outer->y1 && inner->y2 <= outer->y2;
}

static bool area_empty(const lv_area_t *area)
{
    return area->x2 < area->x1 || area->y2 < area->y1;
}

static const lv_obj_t *find_content_root(void)
{
    lv_obj_t *screen = lv_screen_active();
    uint32_t children = lv_obj_get_child_count(screen);

    for (uint32_t i = 0; i < children; i++) {
        lv_obj_t *child = lv_obj_get_child(screen, i);
        lv_area_t area;
        lv_obj_get_coords(child, &area);
        if (area.x1 == 0 && area.y1 == TINYOS_CONTENT_Y &&
            lv_area_get_width(&area) == TINYOS_SCREEN_W &&
            lv_area_get_height(&area) == TINYOS_CONTENT_H)
            return child;
    }
    return NULL;
}

static void report(const char *kind, const lv_obj_t *obj, const char *text,
                   const lv_area_t *box, const lv_area_t *allowed)
{
    failures++;
    if (reported_failures >= MAX_REPORTED_FAILURES)
        return;
    reported_failures++;
    printf("LAYOUT FAIL [%s] %s box=(%d,%d)-(%d,%d) allowed=(%d,%d)-(%d,%d) text=\"%s\"\n",
           audit_name ? audit_name : "?",
           kind, box->x1, box->y1, box->x2, box->y2,
           allowed->x1, allowed->y1, allowed->x2, allowed->y2,
           text ? text : "");
    (void)obj;
}

static void audit_label(lv_obj_t *obj, const lv_area_t *content)
{
    const char *text = lv_label_get_text(obj);
    if (!text || !text[0])
        return;

    const lv_font_t *font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    if (!font)
        return;

    int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    int32_t max_width = LV_COORD_MAX;
    lv_label_long_mode_t mode = lv_label_get_long_mode(obj);
    if (mode == LV_LABEL_LONG_MODE_WRAP || mode == LV_LABEL_LONG_MODE_DOTS)
        max_width = lv_area_get_width(content);

    lv_point_t measured;
    lv_text_get_size(&measured, text, font, letter_space, line_space, max_width,
                     LV_TEXT_FLAG_NONE);
    labels_checked++;

    if (measured.x > lv_area_get_width(content) ||
        measured.y > lv_area_get_height(content)) {
        char kind[96];
        snprintf(kind, sizeof(kind), "label text w=%d h=%d content=%dx%d",
                 (int)measured.x, (int)measured.y,
                 (int)lv_area_get_width(content), (int)lv_area_get_height(content));
        report(kind, obj, text, content, content);
    }
}

static void audit_label_containment(lv_obj_t *label)
{
    lv_area_t box;
    lv_obj_get_coords(label, &box);
    lv_area_t panel = { 0, 0, PANEL_WIDTH - 1, PANEL_HEIGHT - 1 };

    if (area_empty(&box)) {
        report("label empty box", label, lv_label_get_text(label), &box, &panel);
        return;
    }

    /* A virtualized/scrollable ancestor clips its children. A row that has
     * scrolled completely out of that viewport is not drawn and must not be
     * reported just because its stale box sits above the screen. */
    for (lv_obj_t *parent = lv_obj_get_parent(label);
         parent && parent != audit_screen;
         parent = lv_obj_get_parent(parent)) {
        lv_area_t parent_box;
        lv_obj_get_coords(parent, &parent_box);
        if (!area_intersects(&box, &parent_box))
            return;

        /* The content viewport is the installable UI's actual canvas. Check it
         * explicitly so a label cannot escape into the status/navigation bands
         * while still sitting inside the physical 240x240 panel. A label that
         * has been fully scrolled out of its immediate parent returned above. */
        if (parent == audit_content) {
            lv_area_t content_viewport = {
                0, TINYOS_CONTENT_Y, TINYOS_SCREEN_W - 1, TINYOS_CONTENT_BOTTOM
            };
            if (!area_inside(&box, &content_viewport)) {
                report("label outside content", label, lv_label_get_text(label),
                       &box, &content_viewport);
                return;
            }
        }

        if (!area_inside(&box, &parent_box)) {
            report("label outside parent", label, lv_label_get_text(label), &box, &parent_box);
            return;
        }
    }

    if (!area_intersects(&box, &panel))
        return;
    if (!area_inside(&box, &panel))
        report("label outside panel", label, lv_label_get_text(label), &box, &panel);

    lv_area_t content;
    lv_obj_get_content_coords(label, &content);
    audit_label(label, &content);
}

static void audit_object(lv_obj_t *obj)
{
    if (!obj || lv_obj_is_hidden(obj))
        return;

    objects_checked++;

    if (lv_obj_check_type(obj, &lv_label_class))
        audit_label_containment(obj);

    uint32_t children = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < children; i++)
        audit_object(lv_obj_get_child(obj, i));
}

int tinyos_layout_audit(const char *screen_name)
{
    audit_screen = lv_screen_active();
    audit_content = find_content_root();
    audit_name = screen_name;
    failures = 0;
    labels_checked = 0;
    objects_checked = 0;
    reported_failures = 0;

    /* The frame has already been laid out and rendered; do not run a second
     * layout pass here because that can move flex children after the frame the
     * user sees was measured. */
    lv_obj_t *root = lv_screen_active();
    audit_object(root);

    printf("layout audit: screen=%s objects=%d labels=%d failures=%d: %s\n",
           screen_name ? screen_name : "?", objects_checked, labels_checked,
           failures, failures == 0 ? "PASS" : "FAIL");
    return failures == 0 ? 0 : 1;
}

/* A deliberately broken object tree for the permanent negative self-test in
 * tests/layout-audit.sh. Each label exercises one independent rule: glyph
 * width, line height, ancestor containment, content viewport and panel bounds.
 * The test requires all five failures, so a silently disabled audit cannot
 * masquerade as a passing build. */
int tinyos_layout_audit_selftest(void)
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_t *content = (lv_obj_t *)find_content_root();
    if (!content) {
        printf("layout self-test: content viewport not found\n");
        return 2;
    }

    lv_obj_t *text_width = lv_label_create(content);
    lv_obj_set_pos(text_width, 4, 4);
    lv_obj_set_size(text_width, 24, 24);
    lv_obj_set_style_pad_all(text_width, 0, 0);
    lv_obj_set_style_text_font(text_width, &lv_font_montserrat_16, 0);
    lv_label_set_long_mode(text_width, LV_LABEL_LONG_MODE_CLIP);
    lv_label_set_text(text_width, "WWWW");

    lv_obj_t *text_height = lv_label_create(content);
    lv_obj_set_pos(text_height, 4, 34);
    lv_obj_set_size(text_height, 200, 8);
    lv_obj_set_style_pad_all(text_height, 0, 0);
    lv_obj_set_style_text_font(text_height, &lv_font_montserrat_16, 0);
    lv_label_set_long_mode(text_height, LV_LABEL_LONG_MODE_CLIP);
    lv_label_set_text(text_height, "M");

    lv_obj_t *small_parent = lv_obj_create(content);
    lv_obj_set_pos(small_parent, 8, 50);
    lv_obj_set_size(small_parent, 60, 24);
    lv_obj_set_style_pad_all(small_parent, 0, 0);
    lv_obj_set_style_bg_opa(small_parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(small_parent, 0, 0);

    lv_obj_t *outside_parent = lv_label_create(small_parent);
    lv_obj_set_pos(outside_parent, 40, 2);
    lv_obj_set_size(outside_parent, 60, 16);
    lv_obj_set_style_pad_all(outside_parent, 0, 0);
    lv_obj_set_style_text_font(outside_parent, &lv_font_montserrat_10, 0);
    lv_label_set_text(outside_parent, "PARENT");

    lv_obj_t *outside_content = lv_label_create(content);
    lv_obj_set_pos(outside_content, 4, 170);
    lv_obj_set_size(outside_content, 24, 30);
    lv_obj_set_style_pad_all(outside_content, 0, 0);
    lv_obj_set_style_text_font(outside_content, &lv_font_montserrat_10, 0);
    lv_label_set_text(outside_content, "CONTENT");

    /* A direct child of the screen cannot be excused by an ancestor: its
     * box starts inside the panel and ends outside it. */
    lv_obj_t *outside_panel = lv_label_create(screen);
    lv_obj_set_pos(outside_panel, 230, 230);
    lv_obj_set_size(outside_panel, 60, 16);
    lv_obj_set_style_pad_all(outside_panel, 0, 0);
    lv_obj_set_style_text_font(outside_panel, &lv_font_montserrat_10, 0);
    lv_label_set_text(outside_panel, "PANEL");

    lv_obj_update_layout(screen);
    return tinyos_layout_audit("selftest");
}
