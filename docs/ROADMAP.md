# Roadmap

## Done

- Independent TinyOS repository, MIT-licensed original code.
- Custom LVGL 9.6 shell, 25 runtime palettes, 10 animated wallpapers,
  6 typefaces and ten screens.
- Game Boy, Game Boy Color and Game Boy Advance through the vendored
  mGBA 0.10.5 core, with direct RGB565 fbdev output, ALSA audio, evdev input
  and `.sav` battery saves beside each ROM.
- Host simulator with PNG output for every screen.
- Linux 6.12 LTS platform backends: fbdev, evdev, AXP209/sysfs, ALSA WAV.
- Buildroot 2025.02.3 external tree with LVGL and TinyOS packages.
- U-Boot 2025.10 RG Nano fragment and `boot.scr`.
- Kernel fragment for panel, buttons, PMIC, RTC, audio, storage and USB.
- ST7789V panel init sequence and local converter.
- MBR SD image: SPL, FAT boot, ext4 root, FAT data.
- GitHub Actions: simulator screenshots and full Buildroot image.

## Next: hardware bring-up

These are ordered by risk, not by visible feature value.

1. **Panel** — verify init, colour order, inversion, orientation and backlight.
2. **Buttons** — confirm pcal6416 key codes and focus order on real hardware.
3. **Audio** — check card creation, PF6 amplifier enable, mixer range and pop.
4. **Power** — validate AXP209 capacity/charging, power key and shutdown.
5. **Storage** — confirm boot from SD, `/data` mount, settings persistence and
   ROM/save access under `/data/roms`.
6. **Emulation** — measure GBA and GBC frame times on the V3s, verify audio
   clock/pitch, GBA L/R, save reload and that the X + Y chord is reliable under
   long games. Add a measured frame-skip policy only if a title misses 60 Hz.
7. **Thermals** — run the UI, audio and an emulator for 30 minutes and record
   temperature.

## Then: polish

- Boot splash using the same orb/wordmark language.
- USB mass-storage gadget and a small transfer helper.
- Album art from a sidecar image or WAV metadata.
- A now-playing overlay shared by all tabs.
- Additional wallpaper families and per-theme wallpaper pairings.
- Better empty states and a first-run guide.
- Optional screensaver/dim timer with AXP209 backlight control.

## Longer term

- A module SDK with a documented manifest and out-of-tree modules.
- Read-only rootfs with an overlay for atomic updates.
- A/B root partitions or a recovery image.
- More audio formats through a small decoder module.
- A test harness that boots the image under QEMU for headless smoke tests.

## Non-goals

- Android applications.
- A browser or network stack on the stock 64 MB device.
- Proprietary blobs or a vendor kernel fork.
- A desktop-style window manager.

Every item above is optional until the hardware bring-up checklist is complete.
