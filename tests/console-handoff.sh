#!/bin/sh
# Exercise every vtconsole shape the handoff must accept or reject.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
HELPER="$ROOT/buildroot-external/board/rg-nano/rootfs-overlay/etc/tinyos-console-handoff.sh"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
failures=0

reset()
{
    rm -rf "$TMP/vt" "$TMP/fbcon"
    mkdir -p "$TMP/vt"
}

node()
{
    mkdir -p "$TMP/vt/$1"
    printf '%s\n' "$2" > "$TMP/vt/$1/name"
    [ -z "${3:-}" ] || printf '%s\n' "$3" > "$TMP/vt/$1/bind"
}

run_helper()
{
    sh "$HELPER" --vt-dir "$TMP/vt" --fbcon-dir "$TMP/fbcon" \
        > "$TMP/out" 2>&1
}

expect_ok()
{
    name=$1
    reset
    node vtcon0 "${2:-$(printf '(S) dummy device')}" "${3:-}"
    if ! run_helper; then
        echo "FAIL $name: unexpected exit"
        cat "$TMP/out"
        failures=$((failures + 1))
        return
    fi
    if [ -n "${4:-}" ] && [ "$(cat "$TMP/vt/vtcon0/bind" 2>/dev/null)" != "$4" ]; then
        echo "FAIL $name: bind did not become $4"
        failures=$((failures + 1))
        return
    fi
    echo "PASS $name"
}

expect_fail()
{
    name=$1
    reset
    if [ "${3:-__missing__}" = "__missing__" ]; then
        [ -z "${2:-}" ] || node vtcon0 "$2"
    else
        node vtcon0 "$2" "$3"
    fi
    if run_helper; then
        echo "FAIL $name: helper accepted a non-provable handoff"
        cat "$TMP/out"
        failures=$((failures + 1))
    else
        echo "PASS $name"
    fi
}

expect_ok frame-buffer "(S) frame buffer device" 1 0
expect_ok fbcon "(S) fbcon" 1 0
expect_ok FBCon-case "(S) FBCon" 1 0

# The production kernel normally exposes a dummy console plus fbcon; only
# the framebuffer node may be touched.
reset
node vtcon0 "(S) dummy device" 1
node vtcon1 "(S) frame buffer device" 1
if run_helper && [ "$(cat "$TMP/vt/vtcon0/bind")" = 1 ] && [ "$(cat "$TMP/vt/vtcon1/bind")" = 0 ]; then
    echo "PASS dummy-plus-fbcon"
else
    echo "FAIL dummy-plus-fbcon"
    cat "$TMP/out"
    failures=$((failures + 1))
fi
expect_ok framebuffer "(M) framebuffer device" 1 0
expect_ok already-detached "(S) frame buffer device" 0 0
expect_ok dummy-only "(S) dummy device" 1

reset
node vtcon0 "(S) dummy device"
expect_fail unknown-console "(S) virtual terminal" 1
expect_fail missing-bind "(S) frame buffer device"
# A node without a readable name must never be treated as a harmless
# no-console case.
reset
mkdir -p "$TMP/vt/vtcon0"
if run_helper; then
    echo "FAIL missing-name: helper accepted a nameless VT node"
    failures=$((failures + 1))
else
    echo "PASS missing-name"
fi

# Replace the bind file with a directory: cat/write must fail, not be skipped.
reset
node vtcon0 "(S) frame buffer device"
rm -f "$TMP/vt/vtcon0/bind"
mkdir "$TMP/vt/vtcon0/bind"
if run_helper; then
    echo "FAIL directory-bind: helper accepted a directory"
    failures=$((failures + 1))
else
    echo "PASS directory-bind"
fi

# Missing class.
rm -rf "$TMP/vt"
if sh "$HELPER" --vt-dir "$TMP/missing" --fbcon-dir "$TMP/fbcon" >"$TMP/out" 2>&1; then
    echo "FAIL missing-vtconsole: helper accepted a missing class"
    failures=$((failures + 1))
else
    echo "PASS missing-vtconsole"
fi

# A live fbcon sysfs entry with no vtconsole node is not a valid handoff.
reset
mkdir -p "$TMP/fbcon"
if sh "$HELPER" --vt-dir "$TMP/vt" --fbcon-dir "$TMP/fbcon" >"$TMP/out" 2>&1; then
    echo "FAIL fbcon-without-vtconsole: helper accepted an unprovable handoff"
    failures=$((failures + 1))
else
    echo "PASS fbcon-without-vtconsole"
fi

if [ "$failures" -ne 0 ]; then
    echo "console handoff: $failures test(s) failed"
    exit 1
fi
echo "console handoff: all cases PASS"
