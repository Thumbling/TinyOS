#!/bin/sh
# Start the UI. Its log goes to the data partition when that is writable, so a
# black screen can still be explained afterwards. The start must never depend
# on the log: fall back to tmpfs when /data is missing or read-only.
LOG=/data/tinyos.log
if ! : > "$LOG" 2>/dev/null; then
    LOG=/tmp/tinyos.log
fi

# The Games module scans these three directories. They live on the writable
# data volume, so games survive a firmware update; a missing directory is a
# normal empty library, never a startup failure.
mkdir -p /data/roms/gb /data/roms/gbc /data/roms/gba 2>/dev/null || true

# Keep the boot console if the UI binary is missing: the panel is the only
# local diagnostic surface, and taking it away would hide the reason.
if [ ! -x /usr/bin/tinyos ]; then
    echo "error: /usr/bin/tinyos missing or not executable" >> "$LOG" 2>&1
    exit 1
fi

# From here on the panel belongs to the UI. The kernel console was started
# with loglevel=7 so the boot could be watched; left at that level every
# driver message and the blinking text cursor keep repainting the panel over
# the UI. First detach the framebuffer console itself: printk and cursor
# settings are advisory, while an unbound fbcon cannot paint into /dev/fb0 at
# all. A failed or unprovable handoff aborts before the UI can start, so the
# console stays usable and the log says exactly why. Only after the detach
# succeeds do we lower printk and stop the cursor as extra defences.
if ! sh /etc/tinyos-console-handoff.sh >> "$LOG" 2>&1; then
    echo "console: handoff failed; refusing to start UI" >> "$LOG" 2>&1
    exit 1
fi

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
