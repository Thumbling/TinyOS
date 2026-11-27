#!/bin/sh
# Compile and run the actual fbdev clear/blit implementation on the host.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT="$ROOT/build/fbdev-blit-test"
mkdir -p "$ROOT/build"

compile()
{
    if [ -n "${ZIG:-}" ]; then
        "$ZIG" cc "$@"
    else
        ${CC:-cc} "$@"
    fi
}

compile -std=c11 -Wall -Wextra -Werror \
    -I "$ROOT/src/platform/linux" \
    "$ROOT/tests/fbdev-blit-test.c" \
    "$ROOT/src/platform/linux/fbdev_blit.c" \
    -o "$OUT"

"$OUT"
