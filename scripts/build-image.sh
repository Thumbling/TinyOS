#!/bin/sh
# Build the complete TinyOS SD-card image with Buildroot 2025.02.3.
#
# Run this on a Linux machine (or in CI). The script downloads Buildroot,
# applies the TinyOS br2-external tree, and leaves the image in output/images.
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILDROOT_VERSION="2025.02.3"
BUILDROOT_SHA256="a0a7dba32c95e80ea5d5e3b22a56510ead882051c1bf0f7facef8d9cb5b4bb61"
BUILDROOT_URL="https://buildroot.org/downloads/buildroot-${BUILDROOT_VERSION}.tar.gz"

SOURCE_DIR="${BUILDROOT_SRC:-$ROOT/build/buildroot-${BUILDROOT_VERSION}}"
OUTPUT_DIR="${BUILDROOT_OUT:-$ROOT/output}"
EXTERNAL_DIR="$ROOT/buildroot-external"
DOWNLOADS_DIR="${BR2_DL_DIR:-$ROOT/dl}"
ARCHIVE="$ROOT/build/buildroot-${BUILDROOT_VERSION}.tar.gz"
JOBS="${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}"

mkdir -p "$ROOT/build" "$OUTPUT_DIR" "$DOWNLOADS_DIR"

if [ ! -f "$SOURCE_DIR/Makefile" ]; then
    echo "==> downloading Buildroot ${BUILDROOT_VERSION}"
    curl -fL --retry 3 -o "$ARCHIVE" "$BUILDROOT_URL"
    printf '%s  %s\n' "$BUILDROOT_SHA256" "$ARCHIVE" | sha256sum -c -
    mkdir -p "$SOURCE_DIR"
    tar -xf "$ARCHIVE" -C "$SOURCE_DIR" --strip-components=1
fi

export BR2_DL_DIR="$DOWNLOADS_DIR"
export LC_ALL=C

echo "==> configuring TinyOS (${BUILDROOT_VERSION})"
make -C "$SOURCE_DIR" O="$OUTPUT_DIR" BR2_EXTERNAL="$EXTERNAL_DIR" \
    tinyos_rg_nano_defconfig

echo "==> building image with ${JOBS} jobs"
make -C "$SOURCE_DIR" O="$OUTPUT_DIR" BR2_EXTERNAL="$EXTERNAL_DIR" -j"$JOBS"

echo
echo "Image: $OUTPUT_DIR/images/tinyos-rg-nano.img"
ls -lh "$OUTPUT_DIR/images/tinyos-rg-nano.img"
