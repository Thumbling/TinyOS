#!/bin/sh
# Provide a USB CDC-ACM console on the USB-C port. With the cable connected
# the device shows up as a serial port on the PC, which gives a shell even
# when the display stays dark. Every step is best effort: a missing configfs,
# function or UDC must not disturb the boot.

LOG=/data/tinyos-boot.log
if ! : >> "$LOG" 2>/dev/null; then
    LOG=/tmp/tinyos-boot.log
fi
CONFIGFS=/sys/kernel/config
GADGET="$CONFIGFS/usb_gadget/tinyos"

mount -t configfs none "$CONFIGFS" 2>/dev/null

if [ -d "$GADGET" ]; then
    echo "usbgadget: already configured" >> "$LOG"
    exit 0
fi

mkdir -p "$GADGET" 2>/dev/null || {
    echo "usbgadget: configfs unavailable" >> "$LOG"
    exit 0
}

echo 0x1209 > "$GADGET/idVendor" 2>/dev/null
echo 0x0001 > "$GADGET/idProduct" 2>/dev/null

mkdir -p "$GADGET/strings/0x409" 2>/dev/null
echo "TinyOS" > "$GADGET/strings/0x409/manufacturer" 2>/dev/null
echo "TinyOS console" > "$GADGET/strings/0x409/product" 2>/dev/null
echo "tinyos" > "$GADGET/strings/0x409/serialnumber" 2>/dev/null

mkdir -p "$GADGET/configs/c.1/strings/0x409" 2>/dev/null
echo "ACM" > "$GADGET/configs/c.1/strings/0x409/configuration" 2>/dev/null
echo 250 > "$GADGET/configs/c.1/MaxPower" 2>/dev/null

mkdir -p "$GADGET/functions/acm.usb0" 2>/dev/null
ln -s "$GADGET/functions/acm.usb0" "$GADGET/configs/c.1/acm.usb0" 2>/dev/null

UDC="$(ls /sys/class/udc 2>/dev/null | head -n 1)"
if [ -n "$UDC" ]; then
    echo "$UDC" > "$GADGET/UDC" 2>/dev/null
fi

{
    echo "--- usb gadget ---"
    echo "udc=${UDC:-none}"
    ls /sys/class/udc 2>&1
    cat /sys/class/udc/*/state 2>&1
} >> "$LOG" 2>&1
