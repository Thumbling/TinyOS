# Hardware notes

The hardware description below is derived from the mainline Linux device tree
for the Anbernic RG Nano and the upstream board binding. It is a map for
bring-up, not a claim that every peripheral has been measured on a physical
unit.

## Summary

| Item | Details |
|---|---|
| SoC | Allwinner V3s, single Cortex-A7, NEON, 64 MB DDR2 |
| Display | 240 × 240 IPS, ST7789V-class controller on SPI0 |
| PMIC | AXP209 (battery, charger, regulators, power key) |
| RTC | PCF8563 on I2C |
| Buttons | pcal6416 I2C GPIO expander + gpio-keys |
| Audio | V3s codec, speaker amplifier enable PF6 |
| Storage | microSD, U-Boot SPL at 8 KiB |
| USB | OTG capable |
| Debug | UART0 on the USB serial console, 115200 8N1 |
| Network | none |

## Panel

The mainline DTS uses:

```dts
compatible = "saef,sftc154b", "panel-mipi-dbi-spi";
dc-gpios = <&pio 2 0 GPIO_ACTIVE_HIGH>;      /* PC0 */
reset-gpios = <&pio 1 2 GPIO_ACTIVE_HIGH>;   /* PB2 */
spi-max-frequency = <100000000>;
hactive = <240>;
vactive = <240>;
hback-porch = <80>;
```

The controller has a 240 × 320 RAM; the 80-column back porch shifts the visible
240 × 240 window. `panel-mipi-dbi` loads `saef,sftc154b.bin` from firmware. The
init blob is embedded in the kernel (`CONFIG_EXTRA_FIRMWARE`) because the panel
probes before the root filesystem is available.

### Bring-up tuning

These are the first things to check on hardware:

1. **Colour order.** The sequence sets `MADCTL = 0xA0`. If red/blue are
   swapped, use `0xA8` (BGR bit) instead.
2. **Inversion.** `0x21` enables inversion. Remove it if the image is washed
   out or negative.
3. **Orientation and the 80-pixel offset.** `panel-mipi-dbi` has no
   `rotation` property: it passes `rotation = 0` and copies the framebuffer
   unchanged, but it always adds `hback-porch` (80) to the column address
   and `vback-porch` (0) to the row address. The controller only has 320
   addressable columns when `MADCTL` bit 5 (`MV`) is set, so the init
   sequence must use `0x36 0xA0`. With `0x00` the driver writes columns
   80..319 of a 240-column memory, every row shifts and the picture looks
   skewed and flickers. Change the MADCTL value and the porch values
   together, never one alone.
4. **Gamma.** The `0xE0`/`0xE1` tables are the common ST7789V values. If
   contrast is poor, tune them against a test gradient.
5. **Backlight.** The DTS uses the PWM backlight with 12 levels. TinyOS writes a
   percentage into `brightness`; verify the lowest level is still readable.

The text source is `buildroot-external/board/rg-nano/firmware/saef,sftc154b.txt`.
### Frame rate

The fbdev compatibility node is fed through deferred I/O, whose period the
generic DRM code fixes at `HZ / 20` (50 ms, i.e. 20 Hz). With
`CONFIG_HZ=100` that is five jiffies per flush. The board patch
`board/rg-nano/patches/linux/0001-drm-fbdev-dma-flush-once-per-tick.patch`
changes the period of the shadowed fbdev-dma path (the one a dirty-capable
framebuffer like panel-mipi-dbi uses) to a single jiffy, i.e. 10 ms. The
deferred work still merges every write of a jiffy into one SPI transfer, so
the panel sees at most 100 updates/s; a full 240x240 RGB565 frame needs about
10 ms at 100 MHz SPI, so the panel is the limit, not the flush timer. The
kernel logs `tinyos: fbdev-dma deferred IO delay-jiffies=1 (HZ=100)` at probe;
the acceptance check reads that marker out of the built kernel.

Regenerate the binary with:

```sh
python3 tools/mipi_dbi_cmd.py \
  "buildroot-external/board/rg-nano/firmware/saef,sftc154b.txt" \
  "buildroot-external/board/rg-nano/firmware/saef,sftc154b.bin"
```

## Buttons

The pcal6416 exposes the buttons through gpio-keys. The kernel emits standard
`BTN_*` codes; TinyOS maps them in `src/platform/linux/input_evdev.c`.

| Physical | Linux code | TinyOS action |
|---|---|---|
| D-pad up/down/left/right | `BTN_DPAD_*` | focus movement |
| A | `BTN_EAST` | Enter |
| B | `BTN_SOUTH` | Back |
| X | `BTN_NORTH` | Previous |
| Y | `BTN_WEST` | Next |
| L / R | `BTN_TL` / `BTN_TR` | Previous / Next |
| Start | `BTN_START` | Home |
| Select | `BTN_SELECT` | Back |
| Power | `KEY_POWER` | Back (short press) |

If a unit reports different codes, change only the `translate_key()` table.

## Power

- AXP209 battery and charger nodes are enabled by the mainline DTS.
- TinyOS reads `capacity` and `status` from the power-supply class.
- The status bar shows `⚡` while charging and a battery glyph otherwise.
- `poweroff` and `reboot` use the kernel reboot syscall; the AXP209 handles the
  final rail shutdown.
- A long power-key press is handled by the PMIC/kernel. The app maps a short
  `KEY_POWER` event to Back.

## Audio

The V3s codec is enabled in the DTS with:

```dts
allwinner,audio-routing = "Speaker", "HP",
                          "MIC1", "Mic",
                          "Mic", "HBIAS";
allwinner,pa-gpios = <&pio 5 6 (GPIO_ACTIVE_HIGH | GPIO_PULL_UP)>; /* PF6 */
```

The codec driver creates the sound card; no `simple-audio-card` node is needed.
TinyOS opens ALSA `default` (card 0) and applies volume to playback mixer
elements. If the speaker is silent:

1. check `aplay -l` and `alsamixer` over serial;
2. verify PF6 is driven high when playing;
3. check the codec DAPM route with `amixer -c 0 contents`;
4. confirm the WAV is PCM16 mono/stereo, not float or ADPCM.

## Storage and boot

The SD image is MBR:

| Area | Offset | Size | Contents |
|---|---|---|---|
| SPL/U-Boot | 8 KiB | 32 KiB | `u-boot-sunxi-with-spl.bin` (U-Boot proper at 40 KiB) |
| boot | 1 MiB | 64 MiB | FAT: `zImage`, DTB, `boot.scr` |
| root | 65 MiB | 512 MiB | ext4 root filesystem |
| data | 577 MiB | 256 MiB | FAT data volume, mounted at `/data` |

The data partition is intentionally separate from the root filesystem so music
and settings survive a firmware update that rewrites the root partition.

All load addresses stay inside the 64 MB layout U-Boot itself uses on sunxi
(`bootm_size` is 46 MB): `zImage` at 0x41000000, DTB at 0x41800000 and
`boot.scr` at 0x41900000. Do not move them towards the top of RAM: U-Boot
relocates itself there and the DTB would be overwritten while loading.

## Diagnostics without a serial port

The board has no accessible UART, so the image reports on itself:

| Channel | What it proves |
|---|---|
| `UBOOT.TXT` on the data partition | U-Boot ran and can read and write the card |
| `/data/tinyos-boot.log` | the kernel reached userspace; contains `dmesg`, backlight and framebuffer state |
| `/data/tinyos.log` | the UI started, or why it exited |
| Lit panel with kernel messages | DRM panel and backlight probed |
| USB serial port on the PC | the USB gadget came up; a getty runs on `ttyGS0` |

Pull the card and open the FAT `TINYOS_DATA` volume on any PC to read the logs.
The kernel also keeps a text console on the panel (`console=tty0`), so a boot
that stops early is visible on the device itself up to the handoff. Immediately
before starting the UI, `tinyos-run.sh` calls `tinyos-console-handoff.sh`. The helper accepts
the handoff only after it has **unbound and read-back-verified** every
framebuffer VT console through `/sys/class/vtconsole`; only then does the run
script lower printk and stop the cursor. A missing `vtconsole` class, a
missing/read-only `bind` file, a nameless or unknown VT console, or a node that
still reads `bind=1` makes the helper fail, and `tinyos-run.sh` writes
`console: handoff failed; refusing to start UI` to `/data/tinyos.log` and does
not start the binary. The Linux backend checks the same state again immediately
before `open(/dev/fb0)`, so even a manual first start cannot bypass the
handoff; it logs `fbcon handoff not verified; refusing to open /dev/fb0`.

On a successful start `/data/tinyos.log` contains the helper's
`console: detached ...` line, then `fb0 cleared N mapped bytes before first
frame`, the framebuffer geometry line, and finally
`first frame committed: full 240x240 panel`. The app holds
`/tmp/tinyos-ui.lock` while it runs, so a second UI process exits before it can
open the display. The display clears all mapped bytes and the app forces a
synchronous full 240 x 240 refresh after the shell is built. The kernel boots
with `consoleblank=0` and `vt.global_cursor_default=0` as extra defences.
After the handoff, use the serial console or the data-partition logs; the panel
console is intentionally gone.

### Verified where

In-repo tests cover the helper's fake vtconsole variants, source and ARM compile
ordering, the one-opener/one-mmap audit, the host LVGL 57,600-pixel coverage,
and the actual portable fbdev clear/blit code. The real vtconsole names and
`bind` write, the kernel's post-detach printk/VT routing, the physical panel's
first frame, and the runtime lock on the device still require a rebuilt image
flashed to RG-Nano hardware. The existing prebuilt images in this checkout do
not contain these changes.

## Known unknowns

- Panel init sequence and colour order on the actual `saef,sftc154b` unit.
- Whether the pcal6416 interrupt line is wired and useful; TinyOS polls evdev.
- Exact backlight curve and minimum usable brightness.
- Codec gain and speaker pop behaviour at startup.
- Thermal behaviour under sustained UI/audio load.
- USB gadget composition and transfer speed.
- Actual `/sys/class/vtconsole` naming and `bind` behavior of the shipped
  kernel, and whether a rebuilt image's panel remains free of console output
  after the handoff.
- Actual fbdev first-frame appearance and the runtime single-UI lock on a
  physical RG Nano.

These are tracked in [ROADMAP.md](ROADMAP.md).
