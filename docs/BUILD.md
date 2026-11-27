# Building and flashing

## Simulator (any POSIX host)

Requirements: a C compiler, GNU make, Python 3 and the vendored LVGL source.

```sh
make sim
./build/tinyos-sim --screen home --out build/home.png
make shots
```

On Windows, the repository can be built with Zig's C compiler and GNU make:

```sh
make shots HOSTCC="/path/to/zig cc" AR="/path/to/zig ar"
```

The simulator compiles the same `src/app.c`, `src/core`, `src/emulator`,
`src/ui`, `src/modules` and host platform shims as the firmware, together with
both mGBA cores. It writes 240 × 240 PNGs, so visual review does not need a
device. A bounded game smoke run is available without a ROM in the repository:

```sh
TINYOS_SIM_GAME_FRAMES=2 ./build/tinyos-sim \
  --game build/roms/gba/your-test.gba --out build/game.png
```

## Firmware image (Linux)

Requirements:

- a Linux host (x86_64 recommended), about 15 GB free disk;
- common Buildroot host tools: `build-essential`, `bison`, `flex`, `rsync`,
  `cpio`, `unzip`, `bc`, `libssl-dev`, `libncurses-dev`, `libelf-dev`,
  `python3`, `dosfstools`, `mtools`, `e2fsprogs`, `device-tree-compiler`;
- network access to kernel.org, GitHub and buildroot.org.

```sh
sh scripts/build-image.sh
```

The script:

1. downloads Buildroot 2025.02.3 and verifies its SHA-256;
2. loads `buildroot-external/configs/tinyos_rg_nano_defconfig`;
3. builds Linux 6.12.115, U-Boot 2025.10, the root filesystem and the SD image;
4. writes `output/images/tinyos-rg-nano.img`.

Buildroot downloads are kept in `dl/`; the output tree is in `output/`. Both are
ignored by git.

### Manual Buildroot invocation

```sh
make -C build/buildroot-2025.02.3 \
  O="$PWD/output" \
  BR2_EXTERNAL="$PWD/buildroot-external" \
  tinyos_rg_nano_defconfig

make -C build/buildroot-2025.02.3 \
  O="$PWD/output" \
  BR2_EXTERNAL="$PWD/buildroot-external" \
  -j"$(nproc)"
```

### What gets built

| Component | Version | Source |
|---|---|---|
| Linux | 6.12.115 | kernel.org |
| U-Boot | 2025.10 | denx.de |
| Buildroot | 2025.02.3 | buildroot.org |
| LVGL | 9.6.0 | vendored, `third_party/lvgl` |
| mGBA | 0.10.5 | vendored, `third_party/mgba` (MPL-2.0) |
| TinyOS | local | `src/` |

The mGBA core is built directly by the TinyOS Makefiles from
`third_party/mgba/sources.mk`, with both `M_CORE_GBA` and `M_CORE_GB` enabled.
There is no CMake dependency at build time. Buildroot's TinyOS package receives
the source directory through `MGBA_DIR`; the resulting static archive is linked
into `/usr/bin/tinyos`. The core uses mGBA's software renderer, HLE BIOS and
in-emulator save data, so no external BIOS or ROM is installed.

The kernel uses `sunxi_defconfig` plus
`buildroot-external/board/rg-nano/linux.fragment`. The panel init blob is copied
into the kernel tree by a `LINUX_POST_PATCH_HOOKS` hook before the kernel is
built.

## Flashing

Identify the SD card carefully:

```sh
lsblk -o NAME,SIZE,MODEL,TRAN
```

Then write the raw image:

```sh
xz -dk tinyos-rg-nano.img.xz
sudo dd if=tinyos-rg-nano.img of=/dev/sdX bs=4M conv=fsync status=progress
sync
```

Balena Etcher, Raspberry Pi Imager or Rufus can also write the `.img` (not the
`.xz`) to the card. The card must be at least 1 GB; the image is about 833 MiB.

## Serial console

Connect USB-C data to the host and open the UART at **115200 8N1**. The rescue
getty runs on `ttyS0`; the UI is started from inittab by `tinyos-run.sh` and
logs to `/data/tinyos.log` (or `/tmp/tinyos.log`). Before the UI starts, the
script unbinds the panel's framebuffer console so fbcon cannot paint over the
GUI; a failed or unprovable handoff writes
`console: handoff failed; refusing to start UI` and does not execute the UI.
The app repeats the check immediately before opening `/dev/fb0`, and its
`/tmp/tinyos-ui.lock` record lock refuses a second UI process. The serial
console stays available.

Useful checks:

```sh
dmesg | grep -i -E 'panel|mipi|fb0|pca|axp|codec'
ls -l /dev/fb0 /dev/input/event*
cat /sys/class/power_supply/*/capacity
aplay -l
amixer -c 0 contents
tinyos
```

`tinyos` attempts a second UI instance, but the app's `/tmp/tinyos-ui.lock`
record lock makes it exit while the production instance owns the panel. A
manual *first* start also runs the backend's own fbcon check; if the handoff
was not performed it refuses with
`fbcon handoff not verified; refusing to open /dev/fb0`. Stop the inittab
respawn and stop the running UI first for a manual check. The production
instance is started automatically.

## Handoff and first-frame proofs

```sh
sh tests/console-handoff.sh
sh tests/framebuffer-ownership.sh
ZIG="/path/to/zig" sh tests/fbdev-blit.sh
./build/tinyos-sim --verify-first-frame
./build/tinyos-sim --keytest --screen home
```

The first script exercises the framebuffer-console detach against dummy-only,
frame-buffer, fbcon, `FBCon`, `framebuffer`, already-detached, missing-bind,
missing-name, unknown, missing-class and fbcon-without-vtconsole trees. The
second asserts the one-opener/one-mmap, app-side fbcon guard, instance-lock,
first-frame and handoff-before-exec invariants. `fbdev-blit.sh` compiles and
runs the actual `fbdev_blit.c` clear/copy implementation with 32-bpp XRGB and
16-bpp RGB565 mappings. The simulator command seeds the host framebuffer with a
two-colour stale-console pattern before `tinyos_app_init()` and requires every
one of the 57,600 pixels to be written by the forced initial full refresh. It
must print:

```text
fbdev first frame: clear + 57600-pixel blit, 32/16 bpp: PASS
firstframe: 57600 pixels, 57600 written, 0 stale console pixels: PASS
keytest: held A -> 1 click / 1 event, d-pad repeats 3, after release 0: PASS
```

These are in-repo proofs. They do not validate the actual `/sys/class/vtconsole`
names, the real kernel `bind` write, the physical panel, or printk routing after
the handoff. Those require a rebuilt image and an RG Nano.

## Updating

The data partition (`/data`) is separate from the root filesystem. To update:

1. copy music and settings from `/data` if you want to keep them;
2. flash the new image (this replaces boot and root);
3. the data partition is recreated by the image, so restore files afterwards.

A future updater can write only the root partition and preserve `/data`; the
layout was chosen with that in mind.

## Panel firmware

Edit `buildroot-external/board/rg-nano/firmware/saef,sftc154b.txt` and regenerate:

```sh
python3 tools/mipi_dbi_cmd.py \
  "buildroot-external/board/rg-nano/firmware/saef,sftc154b.txt" \
  "buildroot-external/board/rg-nano/firmware/saef,sftc154b.bin"
```

The Buildroot `panel-mipi-dbi-firmware` package also converts the text file for
`/lib/firmware`; the kernel embeds the same binary.

## Troubleshooting

| Symptom | Check |
|---|---|
| black panel | serial log, `CONFIG_EXTRA_FIRMWARE`, panel reset/DC, init blob |
| inverted / wrong colours | MADCTL `0x36`, inversion `0x21` |
| no buttons | `/dev/input/event*`, `evtest`, pcal6416 probe |
| no audio | `aplay -l`, mixer, PF6 amplifier enable, WAV format |
| no battery | AXP209 probe, `power_supply` class, DTS battery node |
| UI exits immediately | run `tinyos` manually and read the log |
| image does not boot | SPL offset 8 KiB, FAT boot partition, `boot.scr`, DTB name |

## CI

`.github/workflows/build.yml` has two jobs:

- **Simulator and UI screenshots** — builds the host simulator, renders all
  screens and uploads them.
- **Buildroot SD image** — installs host tools, caches Buildroot downloads,
  builds the complete image, compresses it with xz and uploads the image,
  checksum and `boot.scr`.

The firmware job is the authoritative ARM build. A local Windows machine cannot
run Buildroot without a Linux environment; use the CI artifact or a Linux host.
