#!/bin/sh
# Start the UI. Its log goes to the data partition when that is writable, so a
# black screen can still be explained afterwards. The start must never depend
# on the log: fall back to tmpfs when /data is missing or read-only.
LOG=/data/tinyos.log
if ! : > "$LOG" 2>/dev/null; then
    LOG=/tmp/tinyos.log
fi

# From here on the panel belongs to the UI. The kernel console was started
# with loglevel=7 so the boot could be watched; left at that level every
# driver message and the blinking text cursor keep repainting the panel over
# the UI. Emergencies still reach the console, and the boot log on the data
# partition already holds the full dmesg.
printf '1 4 1 7' > /proc/sys/kernel/printk 2>/dev/null
if [ -w /sys/class/graphics/fbcon/cursor_blink ]; then
    echo 0 > /sys/class/graphics/fbcon/cursor_blink 2>/dev/null
fi

{
    echo "--- display ---"
    echo "fb virtual_size=$(cat /sys/class/graphics/fb0/virtual_size 2>/dev/null)" \
         "bpp=$(cat /sys/class/graphics/fb0/bits_per_pixel 2>/dev/null)" \
         "stride=$(cat /sys/class/graphics/fb0/stride 2>/dev/null)"
    grep -i spi /proc/interrupts 2>/dev/null
    echo "starting /usr/bin/tinyos"
} >> "$LOG" 2>&1

exec /usr/bin/tinyos >> "$LOG" 2>&1
