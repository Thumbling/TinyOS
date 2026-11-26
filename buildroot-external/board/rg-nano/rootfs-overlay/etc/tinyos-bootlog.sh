#!/bin/sh
# Record how far the boot got on the FAT data partition, which every PC can
# read. The file also tells us whether the panel and its backlight came up.
# Missing or read-only media must not disturb the boot.

LOG=/data/tinyos-boot.log
if ! : > "$LOG" 2>/dev/null; then
    LOG=/tmp/tinyos-boot.log
fi

{
    echo "TinyOS boot $(date -u '+%Y-%m-%dT%H:%M:%SZ' 2>/dev/null)"
    echo "--- dmesg ---"
    dmesg
    echo "--- backlight ---"
    for device in /sys/class/backlight/*; do
        [ -d "$device" ] || continue
        echo "$device max=$(cat "$device/max_brightness" 2>/dev/null) current=$(cat "$device/brightness" 2>/dev/null)"
    done
    echo "--- framebuffer ---"
    ls -l /dev/fb0 2>&1
    echo "--- input ---"
    ls -l /dev/input 2>&1
    echo "--- mmc ---"
    ls -l /dev/mmcblk* 2>&1
} >> "$LOG" 2>&1

# Turn the backlight fully on even if the driver kept a lower default.
for brightness in /sys/class/backlight/*/brightness; do
    [ -w "$brightness" ] || continue
    cat "${brightness%/*}/max_brightness" > "$brightness" 2>/dev/null
done
