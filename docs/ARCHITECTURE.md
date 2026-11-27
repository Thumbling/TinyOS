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
    │ /etc/inittab → tinyos-run.sh (detaches fbcon)
    ▼
/usr/bin/tinyos
    │ LVGL shell
    ▼
Home / Games / Settings / Music / System / About / Appearance / Themes / Wallpapers / Typeface
```

The panel init firmware is embedded in the kernel image so the display can be
probed before the root filesystem is mounted.

## Runtime layers

```text
┌────────────────────────────────────────────┐
│ modules/   Home Games Settings Music …     │  screens and interactions
├────────────────────────────────────────────┤
│ emulator/  ROM scan + mGBA session         │  direct game loop
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
│ LVGL 9.6 / mGBA 0.10.5 / ALSA / Linux      │  upstream stack
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

## Emulator session

Games is the only module that deliberately leaves the LVGL event path. After a
ROM is selected, `tinyos_emulator_run()`:

1. creates an mGBA core for GBA or GB/GBC and loads the file from `/data/roms`;
2. asks the core for its post-load video dimensions and allocates a software
   frame buffer;
3. stops the WAV player, switches the CPU governor hint to performance, opens a
   direct evdev input set and an ALSA PCM stream;
4. runs one mGBA frame, converts the core's `0x00BBGGRR` pixels to RGB565 and
   writes a 240 × 240 image through `tinyos_game_present_rgb565()` — for GBA a
   native 240 × 160 image with 40 px bars, for GB/GBC a 3:2 nearest-neighbour
   scale to 240 × 216 with 12 px bars;
5. drains the core's stereo audio into the PCM stream and paces to 59.7 Hz;
6. on X + Y (or Power), writes battery save data beside the ROM, closes the
   devices and returns; the Games module then rebuilds the shell and forces a
   full refresh before LVGL resumes.

The emulator does not render through LVGL, and the LVGL partial buffers are not
used during a game. That keeps the frame path deterministic and avoids a
partial-refresh cycle for every emulated frame.

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

- first checks `/sys/class/vtconsole` and refuses to open `/dev/fb0` unless every
  framebuffer VT console is already `bind=0`; this also protects a first manual
  start that bypassed `tinyos-run.sh`;
- opens `/dev/fb0` with a short retry loop (the DRM probe can win the race);
- queries `FBIOGET_VSCREENINFO` / `FBIOGET_FSCREENINFO`;
- validates that the mapping's stride and size reach every visible row;
- mmaps the framebuffer and clears every mapped byte before creating LVGL;
- creates an RGB565 LVGL display with two partial buffers;
- copies dirty rectangles for 16 bpp, or expands to XRGB8888 for 32 bpp.

The clear and rectangle copy live in `fbdev_blit.c`, which has no LVGL or Linux
headers. The real backend uses that code directly, so the host
`tests/fbdev-blit.sh` exercises the production pixel path rather than a copy of
it. A successful start logs `fb0 cleared N mapped bytes before first frame`.

The panel's DRM fbdev has deferred dirty-page handling, so no explicit vsync
ioctl is needed. The backend logs geometry and depth at startup.

The board starts the UI through `tinyos-run.sh`, not directly from inittab. It
first runs `tinyos-console-handoff.sh`, which accepts `bind=0` only after
reading it back from every framebuffer VT console; a missing, read-only,
unknown, or still-bound node aborts the start before `exec`. This matters
because `fbcon` shares `/dev/fb0` with LVGL: a cursor blink or a late kernel
message can paint terminal cells over the UI. A successful handoff logs the
`console: detached ...` line. On failure, the helper's reason is followed by
`console: handoff failed; refusing to start UI`; the binary is not executed, so
no UI frame can be painted. The kernel console is then on the dummy VT; the
serial console remains available, and after the handoff the panel belongs to
TinyOS on the in-repo boot path.

`app.c` additionally takes a `/tmp/tinyos-ui.lock` record lock before opening
the display, so a second UI cannot become another writer. The Linux backend
repeats the `fbcon` check immediately before `open(/dev/fb0)`; a manual first
start that bypasses the run script therefore fails closed with
`fbcon handoff not verified; refusing to open /dev/fb0`. The display validates
the visible geometry and clears all mapped framebuffer bytes before LVGL's
first object. After `tinyos_shell_init()` the app invalidates the full active
screen and calls `lv_refr_now()` synchronously, so the first rendered frame
covers all 240 x 240 pixels rather than relying on a first partial-refresh
rectangle. It then logs `first frame committed: full 240x240 panel`.

### Verification boundary

In-repo checks prove the shell handoff variants, the source-level one-opener
and ordering invariants, the host LVGL write coverage, and the fbdev clear/blit
implementation. They do not prove the actual kernel's vtconsole names, its
`bind` semantics, the physical panel, or printk routing after detaching `fbcon`.
Those require a rebuilt image on real RG-Nano hardware. See [BUILD.md](BUILD.md)
and [HARDWARE.md](HARDWARE.md).

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
