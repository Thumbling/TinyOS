#!/bin/sh
# Wait for the USB gadget to create ttyGS0 before starting a console on it, so
# init never respawns a getty for a device that does not exist.
attempt=0
while [ ! -e /dev/ttyGS0 ] && [ "$attempt" -lt 60 ]; do
    sleep 1
    attempt=$((attempt + 1))
done
[ -e /dev/ttyGS0 ] || exit 0
exec /sbin/getty -L ttyGS0 115200 vt100
