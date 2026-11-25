#!/bin/sh
set -eu

: "${TARGET_DIR:?}"
: "${BR2_EXTERNAL_TINYOS_PATH:?}"

install -d "$TARGET_DIR/lib/firmware"
install -m 0644 \
    "$BR2_EXTERNAL_TINYOS_PATH/board/rg-nano/firmware/saef,sftc154b.bin" \
    "$TARGET_DIR/lib/firmware/saef,sftc154b.bin"

install -d "$TARGET_DIR/data/music"
install -d "$TARGET_DIR/data/tinyos"
install -d "$TARGET_DIR/var/log"

printf 'TinyOS 0.1.0\nBuilt %s\n' "$(date -u +%Y-%m-%dT%H:%M:%SZ)" \
    > "$TARGET_DIR/etc/tinyos-version"
chmod 0644 "$TARGET_DIR/etc/tinyos-version"
