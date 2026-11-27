#!/usr/bin/env bash
# Mechanical 240x240 clipping audit for every screen and variant.
#
# The simulator walks the live LVGL object tree after rendering and fails if a
# label (including every icon label) is larger than its content box, leaves its
# immediate container, leaves the 240x240 panel, or is only partially visible
# because a scroll viewport clips it.
#
# Usage: tests/layout-audit.sh [path-to-tinyos-sim]
set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
SIM=${1:-"$ROOT/build/tinyos-sim"}
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tinyos-layout.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

if [ ! -x "$SIM" ]; then
    echo "layout audit: simulator not found: $SIM" >&2
    exit 2
fi

runs=0
failures=0

run_case() {
    label=$1
    shift
    runs=$((runs + 1))
    log="$TMP/$(printf '%04d' "$runs").log"
    if "$SIM" --layout-audit --frames 35 --out "$TMP/case.png" "$@" >"$log" 2>&1; then
        return 0
    fi
    failures=$((failures + 1))
    printf 'layout audit: FAIL %s\n' "$label"
    grep -E 'LAYOUT FAIL|layout audit:' "$log" || tail -20 "$log"
}

screens="home games settings music system about appearance themes wallpapers typeface"

for text in 100 115 130; do
    for screen in $screens; do
        run_case "$screen $text%" --screen "$screen" --text "$text"
    done
done

for text in 100 115 130; do
    for state in empty loading error; do
        run_case "music $state $text%" --screen music --state "$state" --text "$text"
    done
    run_case "power dialog $text%" --screen settings --text "$text" --down 5 --enter
done

for theme in $(seq 0 24); do
    for text in 100 115 130; do
        run_case "theme $theme $text%" --screen themes --theme "$theme" --text "$text"
    done
done

for wallpaper in $(seq 0 9); do
    for text in 100 115 130; do
        run_case "wallpaper $wallpaper $text%" --screen wallpapers \
            --wallpaper "$wallpaper" --text "$text"
    done
done

for font in $(seq 0 5); do
    for text in 100 115 130; do
        for screen in $screens; do
            run_case "font $font $screen $text%" --screen "$screen" \
                --font "$font" --text "$text"
        done
    done
done

for text in 115 130; do
    for down in $(seq 0 5); do
        run_case "settings $text% down $down" --screen settings --text "$text" --down "$down"
    done
    for down in $(seq 0 4); do
        run_case "appearance $text% down $down" --screen appearance --text "$text" --down "$down"
    done
done

for text in 100 130; do
    for down in $(seq 0 5); do
        run_case "typeface $text% down $down" --screen typeface --text "$text" --down "$down"
    done
    for down in 0 1; do
        run_case "wallpapers $text% down $down" --screen wallpapers --text "$text" --down "$down"
    done
    run_case "themes $text% focused preview" --screen themes --text "$text" --down 1
done

# Permanent negative self-test. The simulator injects five broken labels into
# the real screen and must report every independent rule. This catches an audit
# that has silently become a no-op, not just a visual regression in the UI.
negative_status=0
negative_log="$TMP/negative.log"
if "$SIM" --layout-selftest --screen home --frames 8 --out "$TMP/negative.png" \
    >"$negative_log" 2>&1; then
    echo "layout audit: FAIL negative self-test was detected as clean"
    failures=$((failures + 1))
else
    negative_status=$?
fi

if [ "$negative_status" -ne 1 ]; then
    echo "layout audit: FAIL negative self-test exited $negative_status (expected 1)"
    failures=$((failures + 1))
fi

for needle in 'label text w=' 'label outside parent' 'label outside content' \
              'label outside panel'; do
    if ! grep -Fq "$needle" "$negative_log"; then
        echo "layout audit: FAIL negative self-test missed: $needle"
        failures=$((failures + 1))
    fi
done

if ! grep -Eq 'layout audit: screen=selftest .*failures=[1-9][0-9]*: FAIL' \
    "$negative_log"; then
    echo "layout audit: FAIL negative self-test summary did not report failures"
    failures=$((failures + 1))
fi

if [ "$failures" -eq 0 ]; then
    echo "layout audit: negative self-test: injected errors reported, exit 1, PASS"
fi

if [ "$failures" -ne 0 ]; then
    printf 'layout audit: %d/%d cases failed\n' "$failures" "$runs"
    exit 1
fi

printf 'layout audit: %d matrix cases + permanent negative self-test, 0 failures: PASS\n' "$runs"
