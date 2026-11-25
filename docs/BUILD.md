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

The simulator compiles the same `src/app.c`, `src/core`, `src/ui`,
`src/modules` and host platform shims as the firmware. It writes 240 × 240 PNGs,
so visual review does not need a device.

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
| LVGL | 9.6.0 | GitHub |
| TinyOS | local | `src/` |

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
getty runs on `ttyS0`; the TinyOS application runs from inittab and logs to the
console.

Useful checks:

```sh
dmesg | grep -i -E 'panel|mipi|fb0|pca|axp|codec'
ls -l /dev/fb0 /dev/input/event*
cat /sys/class/power_supply/*/capacity
aplay -l
amixer -c 0 contents
tinyos
```

`tinyos` starts a second UI instance; stop the inittab respawn or use it only
for a quick check. The production instance is started automatically.

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
