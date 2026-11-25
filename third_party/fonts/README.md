# UI typefaces

TinyOS uses six typeface families at eight sizes. Montserrat is linked from
LVGL; the other five are converted to static LVGL 4-bpp bitmaps so the RG Nano
never loads or rasterises a font at runtime.

| Family | Source | License |
|---|---|---|
| Montserrat | [LVGL bundled font](https://github.com/lvgl/lvgl) | SIL OFL 1.1 |
| Inter | [rsms/inter](https://github.com/rsms/inter) | SIL OFL 1.1 |
| IBM Plex Sans | [IBM/plex](https://github.com/IBM/plex) | SIL OFL 1.1 |
| Space Grotesk | [floriankarsten/space-grotesk](https://github.com/floriankarsten/space-grotesk) | SIL OFL 1.1 |
| JetBrains Mono | [JetBrains/JetBrainsMono](https://github.com/JetBrains/JetBrainsMono) | SIL OFL 1.1 |
| Atkinson Hyperlegible | [googlefonts/atkinson-hyperlegible](https://github.com/googlefonts/atkinson-hyperlegible) | SIL OFL 1.1 |

The original `.ttf` files are kept in `third_party/fonts/src/`; the generated
C arrays live in `src/ui/fonts/` and are compiled into the firmware. Only the
printable ASCII range (`U+0020`–`U+007E`) is emitted: symbols and icons always
use LVGL's built-in Montserrat font.

## Regenerating

`lv_font_conv` is a development-only dependency. From the repository root:

```sh
for family in inter plex spacegrotesk jetbrainsmono atkinson; do
  for size in 8 10 12 14 16 20 24 28; do
    npx --yes lv_font_conv \
      --font "third_party/fonts/src/${family}.ttf" \
      --size "$size" --bpp 4 --format lvgl \
      --lv-font-name "tinyos_${family}_${size}" \
      --lv-include lvgl/lvgl.h -r 0x20-0x7E \
      --force-fast-kern-format --no-compress \
      -o "src/ui/fonts/tinyos_${family}_${size}.c"
  done
done
```

The checked-in generator version is 1.5.3. It predates LVGL 9.6's
`static_bitmap` field, so each generated public descriptor needs this line
before `.dsc`:

```c
.static_bitmap = 1,
```

Without it LVGL 9.6 treats the uncompressed bitmap as transient and the glyphs
do not render. The checked-in files already contain the field.

## Licenses

The full OFL texts are in `licenses/`. Keep those files with any redistributed
font or generated bitmap.
