#!/bin/sh
# Static guardrails for the panel ownership contract.
#
# This is not a substitute for running the image on the RG Nano. It makes the
# source-level invariants explicit and fails if a future change reintroduces a
# second framebuffer/VT writer or removes the full first-frame path.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
failures=0

fail()
{
    echo "FAIL $*"
    failures=$((failures + 1))
}

opens=$(grep -c 'open(FB_DEVICE' src/platform/linux/display_fbdev.c || true)
[ "$opens" -eq 1 ] || fail "expected exactly one open(FB_DEVICE), found $opens"
all_opens=$(grep -cE '(^|[^[:alnum:]_])open\(' src/platform/linux/display_fbdev.c || true)
[ "$all_opens" -eq 1 ] || fail "display backend has an extra open call ($all_opens)"

maps=$(grep -c 'mmap(NULL, fb_memory_size' src/platform/linux/display_fbdev.c || true)
[ "$maps" -eq 1 ] || fail "expected exactly one framebuffer mmap, found $maps"
all_maps=$(grep -cE 'mmap\(' src/platform/linux/display_fbdev.c || true)
[ "$all_maps" -eq 1 ] || fail "display backend has an extra mmap call ($all_maps)"

if grep -RIn --exclude-dir=build --exclude-dir=third_party \
    --exclude=display_fbdev.c \
    -E 'open\([^)]*(/dev/fb|FB_DEVICE)|write\([^)]*(/dev/fb|fb_memory)|mmap\([^)]*/dev/fb' \
    src sim buildroot-external; then
    fail "another source path opens or writes the framebuffer"
else
    echo "PASS no second framebuffer opener"
fi

if grep -RIn --exclude-dir=build --exclude-dir=third_party \
    -E '/dev/tty[01]|/dev/console' \
    src sim buildroot-external/board/rg-nano/rootfs-overlay; then
    fail "userspace still references tty0/tty1 or /dev/console"
else
    echo "PASS no userspace VT/console writer"
fi

grep -q 'F_WRLCK' src/app.c || fail "missing single-instance write lock"
grep -q 'open("/tmp/tinyos-ui.lock"' src/app.c || fail "missing UI lock file"
grep -q 'lv_obj_invalidate(lv_screen_active())' src/app.c || fail "missing full invalidation"
grep -q 'lv_refr_now(application.display)' src/app.c || fail "missing synchronous first refresh"
grep -q 'tinyos_fbdev_clear(&fb_geometry)' \
    src/platform/linux/display_fbdev.c || fail "missing full mapped framebuffer clear"
grep -q 'fbcon_is_bound()' \
    src/platform/linux/display_fbdev.c || fail "missing app-side fbcon guard"
grep -q 'refusing to open %s' \
    src/platform/linux/display_fbdev.c || fail "missing visible fbcon refusal log"
grep -q 'geometry->size >= visible_bytes' \
    src/platform/linux/fbdev_blit.c || fail "missing visible-framebuffer coverage check"
grep -q 'memset(geometry->memory, 0, geometry->size)' \
    src/platform/linux/fbdev_blit.c || fail "clear does not cover the whole mapping"

if ! grep -q 'written == pixels' sim/main.c; then
    fail "first-frame test does not require complete host write coverage"
fi

if ! awk '
    /take_instance_lock\(\)/ { lock = NR }
    /tinyos_display_create\(\)/ { display = NR }
    END { exit !(lock && display && lock < display) }
' src/app.c; then
    fail "instance lock is not acquired before the display opens"
else
    echo "PASS instance lock precedes display open"
fi

if ! awk '
    /tinyos_shell_init/ { shell = NR }
    /lv_refr_now/ { refresh = NR }
    /first frame committed/ { logged = NR }
    END { exit !(shell && refresh && logged && shell < refresh && refresh < logged) }
' src/app.c; then
    fail "first frame is not logged after the full refresh"
else
    echo "PASS full refresh precedes first-frame log"
fi

if ! awk '
    /tinyos_fbdev_clear\(&fb_geometry\)/ { clear = NR }
    /lv_display_create\(/ { display = NR }
    END { exit !(clear && display && clear < display) }
' src/platform/linux/display_fbdev.c; then
    fail "framebuffer clear is not before LVGL display creation"
else
    echo "PASS framebuffer clear precedes LVGL display"
fi

grep -q 'console: handoff failed; refusing to start UI' \
    buildroot-external/board/rg-nano/rootfs-overlay/etc/tinyos-run.sh \
    || fail "missing visible handoff-failure marker";

if ! awk '
    /tinyos-console-handoff/ { handoff = NR }
    /exec \/usr\/bin\/tinyos/ { ui = NR }
    END { exit !(handoff && ui && handoff < ui) }
' buildroot-external/board/rg-nano/rootfs-overlay/etc/tinyos-run.sh; then
    fail "fbcon handoff is not before UI exec"
else
    echo "PASS handoff precedes UI exec"
fi

if ! awk '
    /fbcon_is_bound\(\)/ { guard = NR }
    /open\(FB_DEVICE/ { open = NR }
    END { exit !(guard && open && guard < open) }
' src/platform/linux/display_fbdev.c; then
    fail "app-side fbcon guard is not before the framebuffer open"
else
    echo "PASS app-side fbcon guard precedes framebuffer open"
fi

if ! grep -q 'sh /etc/tinyos-console-handoff.sh >> "$LOG" 2>&1' \
    buildroot-external/board/rg-nano/rootfs-overlay/etc/tinyos-run.sh; then
    fail "production handoff call is not the fixed, non-overridable invocation"
else
    echo "PASS production handoff uses fixed paths and arguments"
fi

if [ "$failures" -ne 0 ]; then
    echo "framebuffer ownership: $failures check(s) failed"
    exit 1
fi
echo "framebuffer ownership: all checks PASS"
