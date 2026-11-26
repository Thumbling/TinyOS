#!/bin/sh
# Start the UI. Its log goes to the data partition when that is writable, so a
# black screen can still be explained afterwards. The start must never depend
# on the log: fall back to tmpfs when /data is missing or read-only.
LOG=/data/tinyos.log
if ! : > "$LOG" 2>/dev/null; then
    LOG=/tmp/tinyos.log
fi
exec /usr/bin/tinyos >> "$LOG" 2>&1
