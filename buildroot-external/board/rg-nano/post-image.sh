#!/bin/sh
set -eu

BOARD_DIR="$(cd "$(dirname "$0")" && pwd)"
: "${BINARIES_DIR:?}"
: "${TARGET_DIR:?}"
: "${HOST_DIR:?}"
: "${BUILD_DIR:?}"

GENIMAGE="$HOST_DIR/bin/genimage"
if [ ! -x "$GENIMAGE" ]; then
    GENIMAGE="$(command -v genimage)"
fi

printf 'This FAT partition is the writable TinyOS data volume.\n' \
    > "$BINARIES_DIR/data-readme.txt"

# Buildroot's host-uboot-tools normally generates boot.scr from boot.cmd. Keep
# a fallback for builds that only produce the image through this script.
if [ ! -f "$BINARIES_DIR/boot.scr" ]; then
    MKIMAGE="$HOST_DIR/bin/mkimage"
    if [ ! -x "$MKIMAGE" ]; then
        MKIMAGE="$(command -v mkimage)"
    fi
    "$MKIMAGE" -A arm -O linux -T script -C none -n "TinyOS boot" \
        -d "$BOARD_DIR/boot.cmd" "$BINARIES_DIR/boot.scr"
fi

GENIMAGE_TMP="$BUILD_DIR/genimage.tmp"
rm -rf "$GENIMAGE_TMP"
mkdir -p "$GENIMAGE_TMP"

"$GENIMAGE" \
    --rootpath "$TARGET_DIR" \
    --tmppath "$GENIMAGE_TMP" \
    --inputpath "$BINARIES_DIR" \
    --outputpath "$BINARIES_DIR" \
    --config "$BOARD_DIR/genimage.cfg"

(
    cd "$BINARIES_DIR"
    sha256sum tinyos-rg-nano.img > tinyos-rg-nano.img.sha256
)
