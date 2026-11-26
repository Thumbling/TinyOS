# Architecture

## Boot chain

```text
Allwinner BootROM
    │ reads SPL from offset 8 KiB
    ▼
U-Boot SPL
    │ DRAM init, loads U-Boot proper
    ▼
U-Boot 2025.10
    │ boot.scr from FAT partition 1
    ▼
Linux 6.12 LTS
    │ zImage + sun8i-v3s-anbernic-rg-nano.dtb
    │ root=/dev/mmcblk0p2
    ▼
BusyBox init
    │ /etc/inittab
    ▼
/usr/bin/tinyos
    │ LVGL shell
    ▼
Home / Library / Settings / Music / System / About / Appearance / Themes / Wallpapers / Typeface
```

The panel init firmware is embedded in the kernel image so the display can be
probed before the root filesystem is mounted.

## Runtime layers

```text
┌────────────────────────────────────────────┐
│ modules/   Home Apps Settings Music …      │  screens and interactions
├────────────────────────────────────────────┤
│   ui/        shell, theme, widgets, fonts,    │  navigation, focus, visuals,
             wallpaper                      │  animated background
├────────────────────────────────────────────┤
│ app/       lifecycle, settings, audio      │  shared application state
├────────────────────────────────────────────┤
│ core/      settings, logging, file I/O     │  no UI dependencies
├────────────────────────────────────────────┤
│ platform/  host | linux                    │  display, input, power, audio
├────────────────────────────────────────────┤
│ LVGL 9.6 / Linux / ALSA / libc             │  upstream stack
└────────────────────────────────────────────┘
```

`platform.h` is the only hardware boundary. The host platform implements the
same functions with an in-memory framebuffer and a simulated key queue, so the
UI builds and renders without a V3s.

## Application lifecycle

`main()` installs SIGINT/SIGTERM handlers and calls `tinyos_app_run()`:

1. `lv_init()` and `lv_tick_set_cb(tinyos_tick_ms)`.
2. `tinyos_platform_init()`.
3. `tinyos_display_create()` → `/dev/fb0`.
4. `tinyos_input_create()` → `/dev/input/event*`.
5. theme, settings, audio, brightness and cpufreq.
6. `tinyos_shell_init()` creates the screen, status bar, content area and nav.
7. The loop calls `lv_timer_handler()` and sleeps 5 ms.

The loop is deliberately simple: LVGL owns timers, animations and input
processing. There is no busy-wait and no second UI thread. The audio player is
the only worker thread; it never touches LVGL objects.

## Shell and focus model

The shell owns:

- a 240 × 240 screen with a clipped animated wallpaper layer;
- a 26 px status line (time, battery icon and percentage);
- a 174 px content viewport at y = 30;
- a floating 28 px navigation pill at y = 207.

The wallpaper layer is created behind the other shell layers. Theme changes
rebuild the shell; wallpaper style and speed changes replace only the wallpaper
layer through `tinyos_shell_set_wallpaper()`.

Each module receives the content object and builds its own widgets. The shell
creates a fresh `lv_group` per screen, adds the three nav buttons, then adds
module focusables through `tinyos_shell_add_focus()`. The first registered
focusable receives focus, so the module author controls the initial selection.

`LV_KEY_ESC` calls `tinyos_shell_back()`. Detail screens are tracked in a small
back stack, so Settings → Appearance → Themes unwinds one level at a time.
Pressing Home always clears the stack and returns to the Home tab.

## Module model

A module is a static descriptor:

```c
typedef struct tinyos_module {
    const char *id;
    const char *name;
    const char *subtitle;
    const char *icon;
    lv_obj_t *(*create)(tinyos_ctx *ctx, lv_obj_t *parent);
} tinyos_module;
```

The module builds its widgets under `parent` and registers focusables. It may
attach `LV_EVENT_DELETE` callbacks for heap state or timers. The shell deletes
all children with `lv_obj_clean()` when switching screens, so cleanup is
deterministic.

Modules are listed in `src/modules/modules.c`; the linker keeps them because
the descriptors are referenced. Adding a module is two files plus one table
entry; see [MODULES.md](MODULES.md).

## Display

`display_fbdev.c`:

- opens `/dev/fb0` with a short retry loop (the DRM probe can win the race);
- queries `FBIOGET_VSCREENINFO` / `FBIOGET_FSCREENINFO`;
- mmaps the framebuffer and clears it;
- creates an RGB565 LVGL display with two partial buffers;
- copies dirty rectangles for 16 bpp, or expands to XRGB8888 for 32 bpp.

The panel's DRM fbdev has deferred dirty-page handling, so no explicit vsync
ioctl is needed. The backend logs geometry and depth at startup.

## Input

`input_evdev.c` opens up to eight event nodes non-blocking. The read callback
drains all pending events and keeps the last mapped key. It maps:

```text
D-pad          → LV_KEY_UP/DOWN/LEFT/RIGHT
A (BTN_EAST)   → LV_KEY_ENTER
B (BTN_SOUTH)  → LV_KEY_ESC
X / Y, L / R   → TINYOS_KEY_PREV / TINYOS_KEY_NEXT
Start          → LV_KEY_HOME
Select / Power → LV_KEY_ESC
```

LVGL's keypad group only moves focus on `LV_KEY_NEXT/PREV`, so TinyOS handles
the D-pad arrows itself and moves to the nearest focusable object in that
direction. The semantic X/Y keys are delivered to the focused widget for value
changes (theme preview, wallpaper style/speed, sliders, music transport).
B/Start are handled globally from the focused object, so they work even when a
row or button consumes key events. No key repeat is synthesized; focus
animations and click events are handled by LVGL.

## Power and telemetry

`power_linux.c` is pure sysfs/procfs:

- `/sys/class/power_supply/*` for battery capacity and status;
- `/sys/class/backlight/*/{max_brightness,brightness}`;
- `/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`;
- `/proc/meminfo`, `/proc/uptime`, `thermal_zone0/temp`;
- `reboot(RB_POWER_OFF)` / `reboot(RB_AUTOBOOT)` after `sync()`.

Missing files degrade gracefully: the UI shows `--` or a zero value instead of
failing to start.

## Audio

`audio_alsa.c`:

1. Scans `TINYOS_AUDIO_SYSTEM_DIR` and `TINYOS_AUDIO_USER_DIR` for `.wav`.
2. Parses RIFF chunks to validate PCM16 mono/stereo and compute duration.
3. Owns a playback thread; ALSA writes are outside the UI thread.
4. Pause keeps the position; resuming seeks by frames.
5. `next`/`previous` stop, select and restart.
6. Volume is applied to every active playback mixer element.

The public API in `core/audio.h` hides ALSA and the thread. The host shim
implements the same API with a fixed demo track.

## Settings

`settings.ini` lives on the FAT data partition at
`/data/tinyos/settings.ini`. The parser is a small key/value format:

```ini
brightness=85
volume=62
clock_24h=1
performance=1
reduce_motion=0
theme=0
wallpaper=0
wallpaper_speed=2
font_family=0
```

Writes are atomic (`.tmp` + fsync + rename), so a power loss cannot leave a
half-written file. Defaults are applied when the file is missing or invalid.

## Memory and performance budget

| Item | Approx. |
|---|---|
| LVGL heap | 256 KB |
| Draw buffers | 2 × 19 KB (240 × 40 × 2) |
| Audio thread stacks | 2 × 128 KB default |
| WAV chunk buffers | ~8 KB |
| UI object tree | tens of KB |

The UI avoids image decoders and full-screen animations. Most transitions are
200–300 ms opacity/translate animations on a small number of objects.

## Build integration

- `src/Makefile` builds the firmware binary and can be driven by Buildroot.
- `tools/lvgl-build.mk` builds LVGL as a static archive for host and target.
- `buildroot-external/package/lvgl` installs the archive and headers into staging.
- `buildroot-external/package/tinyos` builds the app and embeds panel firmware.
- `buildroot-external/board/rg-nano` owns U-Boot, kernel, image and rootfs.

This separation keeps the UI buildable on any POSIX host while the image stays
reproducible in Buildroot.
