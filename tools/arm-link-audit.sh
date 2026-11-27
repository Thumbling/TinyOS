#!/usr/bin/env bash
# Build and audit the production emulator path for the RG Nano's ARM target.
#
# The host has no Buildroot sysroot or alsa-lib for ARM. The real emulator,
# session, input, fbdev, power and LVGL code are still compiled for ARM; only
# the ALSA ABI is supplied by tests/arm-link/alsa_stub.c. The final executable
# is a statically linked ARM ELF and must have zero undefined symbols.
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$ROOT"

ZIG=${ZIG:-zig}
AR=${AR:-ar}
MAKE=${MAKE:-make}
NM=${NM:-}
READELF=${READELF:-}
TARGET=${ARM_TARGET:-arm-linux-musleabihf}
CPU=${ARM_CPU:-cortex_a7}
MGBA_BUILD=${MGBA_ARM_BUILD:-build/mgba-arm}
MGBA_LIB="$MGBA_BUILD/libmgba.a"
LVGL_BUILD=${LVGL_ARM_BUILD:-build/lvgl-arm}
LVGL_LIB="$LVGL_BUILD/liblvgl.a"
BUILD=build/arm-link
OBJ="$BUILD/full-obj"
BIN="$BUILD/tinyos-emulator-full"
REPORT="$BUILD/arm-link-audit.txt"

rm -rf "$OBJ"
mkdir -p "$OBJ"

resolve_tool() {
    local name=$1 explicit=$2
    if [ -n "$explicit" ]; then
        command -v "$explicit" >/dev/null 2>&1 && { command -v "$explicit"; return; }
    fi
    if command -v "$name" >/dev/null 2>&1; then
        command -v "$name"; return
    fi
    if command -v "$name.exe" >/dev/null 2>&1; then
        command -v "$name.exe"; return
    fi
    local ar_dir
    ar_dir=$(dirname "$(command -v "$AR" 2>/dev/null || true)")
    if [ -n "$ar_dir" ] && [ -x "$ar_dir/$name.exe" ]; then
        echo "$ar_dir/$name.exe"; return
    fi
    return 1
}

NM=$(resolve_tool nm "$NM") || { echo "missing nm (set NM=...)" >&2; exit 1; }
READELF=$(resolve_tool readelf "$READELF") || { echo "missing readelf (set READELF=...)" >&2; exit 1; }

echo "== arm link audit: $TARGET ($CPU) =="
command -v "$ZIG" >/dev/null 2>&1 || { echo "missing compiler: $ZIG" >&2; exit 1; }
command -v "$AR" >/dev/null 2>&1 || { echo "missing archiver: $AR" >&2; exit 1; }

if [ ! -f "$MGBA_LIB" ]; then
    echo "building mGBA for ARM..."
    "$MAKE" -j4 mgba-host \
        HOSTCC="$ZIG cc -target $TARGET -mcpu=$CPU" \
        AR="$AR" \
        MGBA_BUILD="$MGBA_BUILD" \
        MGBA_LIB="$MGBA_LIB" \
        MGBA_OS_SOURCES="src/platform/posix/memory.c src/util/vfs/vfs-dirent.c" \
        MGBA_OS_CFLAGS="-D_GNU_SOURCE -DHAVE_LOCALE"
fi

if [ ! -f "$LVGL_LIB" ]; then
    echo "building LVGL for ARM..."
    "$MAKE" -j4 -f tools/lvgl-build.mk \
        LVGL_DIR=third_party/lvgl \
        LVGL_BUILD_DIR="$LVGL_BUILD" \
        LVGL_LIB="$LVGL_LIB" \
        CC="$ZIG cc -target $TARGET -mcpu=$CPU" \
        AR="$AR" \
        CFLAGS="-std=gnu11 -O2 -g -w" \
        LVGL_CFLAGS="-DLV_CONF_INCLUDE_SIMPLE -Iconfig"
fi

CFLAGS=(
    -target "$TARGET" -mcpu="$CPU" -std=gnu11 -O2 -g -w
    -D_GNU_SOURCE -DLV_CONF_INCLUDE_SIMPLE
    -Itests/arm-link -Isrc -Iconfig -Ithird_party
    -Ithird_party/lvgl -Ithird_party/lvgl/include
    -Ithird_party/mgba/include -Ithird_party/mgba/src
)

OBJECTS=()
SOURCES=(
    src/emulator/emulator.c
    src/emulator/session.c
    src/emulator/rom.c
    src/emulator/input_map.c
    src/core/log.c
    src/core/key_repeat.c
    src/core/settings.c
    src/core/util.c
    src/platform/linux/input_evdev.c
    src/platform/linux/display_fbdev.c
    src/platform/linux/audio_alsa.c
    src/platform/linux/power_linux.c
    src/platform/linux/fbdev_blit.c
    tests/arm-link/full_probe.c
    tests/arm-link/alsa_stub.c
)

echo "compiling production emulator and platform glue..."
for f in "${SOURCES[@]}"; do
    o="$OBJ/${f//\//_}.o"
    "$ZIG" cc "${CFLAGS[@]}" -c "$f" -o "$o"
    OBJECTS+=("$o")
done

echo "linking static ARM executable..."
"$ZIG" cc -target "$TARGET" -mcpu="$CPU" -static -no-pie -o "$BIN" \
    "${OBJECTS[@]}" "$LVGL_LIB" "$MGBA_LIB" -lm

FILE_OUT=$(file "$BIN")
"$READELF" -h "$BIN" 2>/dev/null | grep -q 'Machine:.*ARM' || {
    echo "FAIL: readelf cannot confirm the ARM machine type" >&2
    exit 1
}
SYMBOL_DUMP=$("$READELF" -sW "$BIN" 2>/dev/null || true)
[ -n "$SYMBOL_DUMP" ] || { echo "FAIL: readelf returned no symbol table" >&2; exit 1; }
# readelf's binding filter is the exact test: an undefined GLOBAL/WEAK symbol
# would need a definition. _DYNAMIC is a LOCAL HIDDEN linker placeholder in
# Zig's static musl startup (there is no dynamic section), so it is not an
# unresolved emulator or platform symbol. nm is still used for the defined
# production-symbol checks below.
UNDEFINED=$(printf '%s\n' "$SYMBOL_DUMP" | awk '$7 == "UND" && $5 != "LOCAL" {print}' || true)
NM_UNDEFINED=$("$NM" -u "$BIN" 2>/dev/null || true)
DYNAMIC=$("$READELF" -d "$BIN" 2>/dev/null | grep -c NEEDED || true)
DYNAMIC_SECTION=$("$READELF" -SW "$BIN" 2>/dev/null | grep -c '\.dynamic' || true)

case "$FILE_OUT" in
    *"ELF 32-bit"*"ARM"*) ;;
    *) echo "FAIL: not an ARM 32-bit ELF: $FILE_OUT" >&2; exit 1 ;;
esac
if [ -n "$UNDEFINED" ]; then
    echo "FAIL: unresolved external symbols in final ARM link:" >&2
    printf '%s\n' "$UNDEFINED" >&2
    exit 1
fi
if [ "$DYNAMIC" != "0" ] || [ "$DYNAMIC_SECTION" != "0" ]; then
    echo "FAIL: final ARM executable has dynamic dependencies or a dynamic section" >&2
    "$READELF" -d "$BIN" | grep NEEDED >&2 || true
    exit 1
fi

for symbol in \
    tinyos_emulator_run \
    tinyos_emulator_session_open \
    tinyos_game_present_rgb565 \
    tinyos_game_input_poll \
    tinyos_game_audio_write; do
    if ! "$NM" --defined-only "$BIN" | grep -q "[[:space:]]$symbol$"; then
        echo "FAIL: production symbol missing from ARM link: $symbol" >&2
        exit 1
    fi
done

{
    echo "TinyOS ARM emulator link audit"
    echo "target: $TARGET ($CPU)"
    echo "compiler: $("$ZIG" version)"
    echo "executable: $BIN"
    echo "file: $FILE_OUT"
    echo "undefined external symbols: 0"
    if [ -n "$NM_UNDEFINED" ]; then
        echo "nm -u (toolchain-local entries, not external references):"
        printf '  %s\n' "$NM_UNDEFINED"
    else
        echo "nm -u: empty"
    fi
    echo "dynamic NEEDED entries: 0"
    echo "dynamic sections: 0"
    echo "cold-start rule: build/arm-link/full-obj is removed before every compile"
    echo "prebuilt ARM archives: optional; rebuilt automatically when missing"
    echo "stale objects: excluded; link uses an explicit object list"
    echo "mgba archive: $MGBA_LIB"
    echo "lvgl archive: $LVGL_LIB"
    echo "ALSA: link-audit ABI stub (tests/arm-link/alsa_stub.c); not alsa-lib"
    echo
    echo "Residual uncertainty:"
    echo "- no target alsa-lib or Buildroot sysroot is available on this host"
    echo "- fbdev/evdev/power ioctls are linked but not executed"
    echo "- tinyos_ctx_get is stubbed; the app/UI context is outside this link"
    echo "- the ARM executable was inspected, not executed on hardware"
    echo
    echo "Linked production translation units:"
    printf '  %s\n' "${SOURCES[@]}"
    echo
    file "$BIN"
} > "$REPORT"

echo "PASS: ARM static link, 0 undefined symbols, 0 dynamic dependencies"
echo "report: $REPORT"
cat "$REPORT"
