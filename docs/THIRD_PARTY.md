# Third-party notices

TinyOS original code is MIT-licensed. The firmware also contains or links the
following upstream components; their licenses remain in effect.

| Component | License | How it is used |
|---|---|---|
| [LVGL 9.6](https://lvgl.io/) | MIT | vendored in `third_party/lvgl`, linked statically |
| [stb_image_write](https://github.com/nothings/stb) | Public domain / MIT | simulator PNG writer |
| [Montserrat](https://fonts.google.com/specimen/Montserrat) | SIL OFL 1.1 | LVGL built-in font bitmaps |
| [Inter](https://github.com/rsms/inter) | SIL OFL 1.1 | generated 4-bpp UI bitmaps |
| [IBM Plex Sans](https://github.com/IBM/plex) | SIL OFL 1.1 | generated 4-bpp UI bitmaps |
| [Space Grotesk](https://github.com/floriankarsten/space-grotesk) | SIL OFL 1.1 | generated 4-bpp UI bitmaps |
| [JetBrains Mono](https://github.com/JetBrains/JetBrainsMono) | SIL OFL 1.1 | generated 4-bpp UI bitmaps |
| [Atkinson Hyperlegible](https://github.com/googlefonts/atkinson-hyperlegible) | SIL OFL 1.1 | generated 4-bpp UI bitmaps |
| [Linux kernel](https://www.kernel.org/) | GPL-2.0 | built by Buildroot, not vendored |
| [U-Boot](https://www.denx.de/wiki/U-Boot) | GPL-2.0+ | bootloader, not vendored |
| [Buildroot](https://buildroot.org/) | GPL-2.0 | build system, not vendored |
| [ALSA lib](https://www.alsa-project.org/) | LGPL-2.1+ | target audio library |
| [BusyBox](https://busybox.net/) | GPL-2.0 | target userland |
| [panel-mipi-dbi](https://github.com/notro/panel-mipi-dbi) | CC0-1.0 (command tool) | init format and reference |

## Assets

- `assets/music/tinyos-theme.wav` is an original 4-second ambient cue generated
  for this project from sine waves. It is released under **CC0-1.0**.
- `buildroot-external/board/rg-nano/firmware/saef,sftc154b.bin` is generated from
  the text sequence in the same directory by `tools/mipi_dbi_cmd.py`; both are
  original TinyOS files.
- The LVGL Montserrat font bitmaps are distributed with LVGL under the SIL Open
  Font License.
- The five generated UI families keep their original TTFs in
  `third_party/fonts/src/`, their OFL texts in `third_party/fonts/licenses/`
  and their generated bitmaps in `src/ui/fonts/`. See
  `third_party/fonts/README.md` for provenance and regeneration.

## No inherited code

The TinyOS source tree does not include code, names or assets from any earlier
firmware project. The application, UI, platform backends, Buildroot external
tree and board files were written for this repository. Upstream components are
used through their public APIs and are credited above.

## License texts

Full license texts for vendored components are kept next to their sources:

- `third_party/lvgl/LICENCE.txt`
- `third_party/stb_image_write.h` (header contains the dual-license notice)
- `third_party/fonts/licenses/` (SIL OFL 1.1 for all five UI families)
- Linux, U-Boot, Buildroot and ALSA license files are provided by their
  upstream source trees during the Buildroot build.
