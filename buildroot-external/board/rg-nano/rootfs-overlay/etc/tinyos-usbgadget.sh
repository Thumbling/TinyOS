#!/bin/sh
# Provide a USB CDC-ACM console on the USB-C port. With the cable connected
# the device shows up as a serial port on the PC, which gives a shell even
# when the display stays dark.

LOG=/data/tinyos-boot.log
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

echo 0x1209 > "$GADGET/idVendor"
echo 0x0001 > "$GADGET/idProduct"

mkdir -p "$GADGET/strings/0x409"
echo "TinyOS" > "$GADGET/strings/0x409/manufacturer"
echo "TinyOS console" > "$GADGET/strings/0x409/product"
echo "tinyos" > "$GADGET/strings/0x409/serialnumber"

mkdir -p "$GADGET/configs/c.1/strings/0x409"
echo "ACM" > "$GADGET/configs/c.1/strings/0x409/configuration"
echo 250 > "$GADGET/configs/c.1/MaxPower"

mkdir -p "$GADGET/functions/acm.usb0"
ln -s "$GADGET/functions/acm.usb0" "$GADGET/configs/c.1/acm.usb0" 2>/dev/null

UDC="$(ls /sys/class/udc 2>/dev/null | head -n 1)"
if [ -n "$UDC" ]; then
    echo "$UDC" > "$GADGET/UDC"
fi

{
    echo "--- usb gadget ---"
    echo "udc=${UDC:-none}"
    ls /sys/class/udc 2>&1
    cat /sys/class/udc/*/state 2>&1
} >> "$LOG" 2>&1
