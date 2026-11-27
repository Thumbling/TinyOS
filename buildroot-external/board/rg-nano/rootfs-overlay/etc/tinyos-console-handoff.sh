#!/bin/sh
# Detach every framebuffer VT console from the panel before the UI starts.
#
# fbcon and LVGL share /dev/fb0. Lowering printk or stopping the cursor is not
# enough: fbcon can still repaint the panel on a VT update, and a failed sysfs
# write must not be mistaken for a successful handoff. This script therefore
# accepts only a verified bind=0, and treats an unknown VT console as a hard
# failure instead of quietly starting the UI with a second writer.
#
# The default paths are the production interface. The test suite passes
# --vt-dir/--fbcon-dir so it can exercise variants without an environment
# override that a caller could use to bypass the handoff in production.
VT_DIR=/sys/class/vtconsole
FBCON_DIR=/sys/class/graphics/fbcon
while [ $# -gt 0 ]; do
    case "$1" in
        --vt-dir)
            [ $# -ge 2 ] || { echo "console: --vt-dir needs a path"; exit 2; }
            VT_DIR=$2
            shift 2
            ;;
        --fbcon-dir)
            [ $# -ge 2 ] || { echo "console: --fbcon-dir needs a path"; exit 2; }
            FBCON_DIR=$2
            shift 2
            ;;
        *)
            echo "console: unknown handoff argument '$1'"
            exit 2
            ;;
    esac
done

if [ ! -d "$VT_DIR" ]; then
    echo "console: $VT_DIR is missing; cannot prove panel ownership"
    exit 1
fi

found_fbcon=0
for vtcon in "$VT_DIR"/vtcon*; do
    [ -d "$vtcon" ] || continue

    if ! name=$(cat "$vtcon/name" 2>/dev/null); then
        echo "console: cannot read $vtcon/name"
        exit 1
    fi

    case "$name" in
        *[Dd]ummy*)
            # A dummy console keeps text in memory but never touches fb0.
            ;;
        *[Ff]rame*[Bb]uffer*|*[Ff]ramebuffer*|*fbcon*|*FBCon*)
            found_fbcon=1

            if [ ! -w "$vtcon/bind" ]; then
                echo "console: $vtcon/bind is missing or read-only"
                exit 1
            fi
            if ! state=$(cat "$vtcon/bind" 2>/dev/null); then
                echo "console: cannot read $vtcon/bind"
                exit 1
            fi
            if [ "$state" != "0" ]; then
                if ! echo 0 > "$vtcon/bind" 2>/dev/null; then
                    echo "console: cannot unbind ${vtcon##*/}"
                    exit 1
                fi
                if ! state=$(cat "$vtcon/bind" 2>/dev/null); then
                    echo "console: cannot verify ${vtcon##*/}/bind"
                    exit 1
                fi
            fi
            if [ "$state" != "0" ]; then
                echo "console: ${vtcon##*/} is still bound (bind=$state)"
                exit 1
            fi
            echo "console: detached ${vtcon##*/} ($name)"
            ;;
        *)
            echo "console: unknown VT console '$name' in $vtcon"
            exit 1
            ;;
    esac
done

if [ "$found_fbcon" -eq 0 ]; then
    # No vtconsole node is not enough on its own: a stale fbcon sysfs entry
    # would mean the driver is present but we did not find its bind control.
    if [ -e "$FBCON_DIR" ]; then
        echo "console: fbcon sysfs exists but no framebuffer vtconsole was found"
        exit 1
    fi
    echo "console: no framebuffer VT console bound"
fi

exit 0
