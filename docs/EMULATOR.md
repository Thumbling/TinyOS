# Game Boy, Game Boy Color and Game Boy Advance

TinyOS uses mGBA 0.10.5 as one core for all three systems. The shared session
API in `src/emulator/session.c` is used by both the production wrapper and the
host proof, so the proof exercises the same composition, save and input code as
the device.

## Host proof

```sh
make emulator-selftest
```

The target generates deterministic 32 KiB test ROMs under the ignored
`build/emulator-selftest/roms/` directory, then runs the headless harness. It
does not need a commercial ROM, a BIOS file, a display, ALSA or an input device.

For each system the harness checks:

- the core geometry after a fixed frame count: 160x144 for GB/GBC and 240x160
  for GBA;
- concrete RGB565 panel values (including channel order) for the test image;
- exact nearest-neighbour scaling and letterboxing in the 240x240 panel:
  240x216 for GB/GBC with 12 px top and bottom bars, and 240x160 for GBA with
  40 px bars;
- `<rom>.sav` creation and a two-run save round trip (8 KiB for GB/GBC, 32 KiB
  for GBA);
- all ten device buttons mapping to the expected core input bits, with X/Y not
  leaking into the emulated console;
- the X+Y exit gesture firing exactly once after 900 ms, not at 899 ms and not
  again while held;
- the 59.7275 Hz rational pacing and the matching audio sample reference.

The current result is `316 checks, all PASS`.

## Cold-start reproducibility

A complete proof starts with no generated build tree (no host or ARM archives,
no test ROMs, no saves and no screenshots):

```sh
rm -rf build
make emulator-selftest
make arm-link-audit ZIG=/path/to/zig AR=/path/to/ar
make -j4 sim
bash tests/layout-audit.sh
./build/tinyos-sim --verify-first-frame
./build/tinyos-sim --keytest --screen home
```

All steps regenerate what they need. The test-ROM generator creates its output
directories, the ARM audit builds mGBA and LVGL when their ARM archives are
missing, and `sim` builds the host LVGL/mGBA libraries. With no `build/roms`
fixture, the Games screen exercises its empty-state layout; that path was
adjusted so the 130% text variant fits without clipping. The same sequence was
then repeated without cleaning, and in a different order, with the same results.

`build/arm-link/full-obj` is removed before every ARM audit compile and the
link uses an explicit object list, so stale objects cannot survive a source
change. Everything generated is under the ignored `build/` directory; no ROM,
BIOS or save file is written into the repository.

## ARM link audit

```sh
make arm-link-audit ZIG=/path/to/zig AR=/path/to/ar
```

The audit compiles the real production pieces for the RG Nano target
(`arm-linux-musleabihf`, Cortex-A7), including `emulator.c`, `session.c`, the
ROM and input mapping code, the Linux evdev/fbdev/power/ALSA glue, LVGL 9.6 and
mGBA. It produces `build/arm-link/tinyos-emulator-full`, a statically linked
32-bit ARM ELF, and records its result in
`build/arm-link/arm-link-audit.txt`.

The check requires zero undefined external symbols (`readelf` binding filter:
no undefined `GLOBAL`/`WEAK` symbol) and no dynamic `NEEDED` entries. It also
requires the production symbols (`tinyos_emulator_run`,
`tinyos_emulator_session_open`, `tinyos_game_present_rgb565`,
`tinyos_game_input_poll` and `tinyos_game_audio_write`) to be defined in the
final link. `nm -u` may report the linker-local hidden `_DYNAMIC` placeholder
emitted by Zig's static musl startup; it is not an external reference and the
binary has no dynamic section.

### Remaining uncertainty

This is a real ARM compile and link of the production emulator and platform
sources, but it is not a hardware test:

- The host has no Buildroot sysroot or ARM `alsa-lib`. `audio_alsa.c` is
  compiled and linked against the ABI declarations and link-only stubs in
  `tests/arm-link/`; actual ALSA symbol versions and runtime behavior still
  need a target sysroot and the RG Nano.
- The fbdev/evdev/power code is compiled and linked but its ioctls and device
  paths cannot run on this Windows host.
- The ARM executable was inspected, not executed, because no ARM user-mode
  runner or RG Nano is attached.
- LVGL and mGBA are compiled from the repository sources for ARM; the full
  Buildroot image remains the final integration check.

Those gaps do not hide unresolved emulator/platform references: the link
audit fails on any undefined external symbol.
