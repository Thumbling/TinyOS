# UI and design system

## Intent

TinyOS should feel like a small, deliberate device — not a desktop shrunk onto a
240 px screen. The interface has one focus, one obvious primary action and a
calm surface hierarchy. Personalisation is a first-class feature: 25 palettes,
10 animated wallpapers and 6 typefaces can be changed without leaving the
device.

The canvas is always **240 × 240**. Nothing is scaled, downsampled or rendered
through an intermediate buffer. Text, icons, hairlines and focus rings are drawn
at native resolution.

## Canvas

| Property | Value |
|---|---|
| Resolution | 240 × 240 |
| Status line | y = 0…26 |
| Content | y = 30…204 (174 px high) |
| Navigation | y = 207…235 |
| Safe margin | 14 px left/right |
| Corner radius | 10–20 px |
| Border | 1 px, low-opacity line colour |

Every screen builds inside the 174 px content viewport. New screens must not
add a fourth shell layer or draw over the status/navigation bars.

## Colour roles

Colours are **runtime tokens**, not compile-time constants. `tinyos_theme_apply()`
switches the active palette, refreshes the shared styles and the shell rebuilds
its background. Modules use semantic roles only:

| Token | Role |
|---|---|
| `BG` | deepest background |
| `BG_2` | background gradient top / navigation base |
| `SURFACE` | cards and rows |
| `SURFACE_2` | raised surfaces, selected rows, pills |
| `LINE` | hairlines and borders |
| `TEXT` | primary text |
| `MUTED` | secondary text |
| `DIM` | metadata and hints |
| `CYAN` / accent | focus, active navigation, primary action |
| `CYAN_DEEP` | gradient partner for artwork |
| `VIOLET` / accent 2 | media and secondary highlights |
| `GREEN` | positive state |
| `AMBER` | attention / settings |
| `RED` | destructive actions |
| `ON_ACCENT` | text/icon colour on an accent fill |

### The 25 palettes

`Abyss`, `Aurora`, `Ember`, `Rosewood`, `Cobalt`, `Lavender`, `Forest`,
`Sandstone`, `Nord`, `Gruvbox`, `Dracula`, `Tokyo Night`, `Solarized`,
`Monochrome`, `Paper`, `Porcelain`, `Mint Light`, `Solar Flare`, `Cyberpunk`,
`Vaporwave`, `Terminal`, `Amber CRT`, `Ice`, `Cosmos`, `Candy`.

Nineteen are dark (including the strictly monochrome palette); `Paper`,
`Porcelain`, `Mint Light` and `Solar Flare` are light. Every foreground/background
pair used for text, metadata, icons and buttons is checked for at least
**4.5:1** contrast on both `SURFACE` and `SURFACE_2`. `ON_ACCENT` is checked
against the accent fill.

Theme preview is a live miniature: it updates its gradient, sample cards, text
and accent bar as the user steps through the gallery. Applying a theme rebuilds
the shell once, so no old colours or fonts remain behind.

## Typography

Six families, each converted to static 4-bpp LVGL bitmaps at **8, 10, 12, 14,
16, 20, 24 and 28 px**:

| Family | Character |
|---|---|
| Montserrat | default; geometric and friendly |
| Inter | neutral, compact UI text |
| IBM Plex Sans | technical, slightly narrow |
| Space Grotesk | expressive display text |
| JetBrains Mono | coding / high-contrast monospaced |
| Atkinson Hyperlegible | maximum character differentiation |

Only printable ASCII (`U+0020`–`U+007E`) is generated for the five custom
families. All symbols and icons always use LVGL's built-in Montserrat, so a
typeface change can never remove a battery, arrow or navigation glyph.

Default sizes:

| Size | Use |
|---|---|
| 28 | home wordmark |
| 24 | about wordmark, hero letter |
| 20 | screen titles |
| 16 | system values, card values |
| 14 | body, status time |
| 10 | labels, values, secondary copy |
| 8 | eyebrows, hints, metadata |

No text is smaller than 8 px. Long labels use `LV_LABEL_LONG_MODE_DOTS`; only
deliberate multi-line copy opts into wrapping. Typeface changes rebuild the
shell and are stored in `/data/tinyos/settings.ini`.

## Screen patterns

### Home

A 212 × 94 hero card with greeting, wordmark, two-line tagline, accent CTA and
a gradient orb. Three 64 × 50 quick tiles (Music, Library, Settings) sit below
a “QUICK ACCESS” eyebrow. The wallpaper remains visible around the cards but
never behind body text without a solid surface.

### Library

A 2 × 2 grid of 102 × 56 app tiles with a 28 px badge, title and subtitle. The
focused tile gets a 2 px accent ring. The footer is metadata, not a control.

### Settings

Seven compact 20 px rows with 1 px gaps: **Appearance**, Brightness, Volume,
24-hour clock, Performance, About TinyOS and Power off. All seven fit without
scrolling. Sliders and switches live inline. Power off is red and opens a
confirmation overlay. Appearance is always the first row so personalisation is
discoverable.

### Appearance

Four 24 px rows: Theme, Wallpaper, Motion and Typeface. Each row shows its
current value in a compact pill; A opens the corresponding picker, B returns to
Settings.

### Themes

A live 212 × 62 preview card, palette name, `NN / 25` counter and a hint line.
X/Y or L/R steps through the gallery; A applies immediately and rebuilds the
UI. The preview never uses the old palette for its sample cards.

### Wallpapers

A 212 × 58 animated preview above three 20 px rows: Wallpaper, Speed and Reduce
motion. Style and speed cycle on A; X/Y or L/R adjusts the focused row. Changes
are saved and applied to the shell wallpaper immediately, so the preview and
the real background never drift apart.

### Typeface

Six 20 px rows. Each row renders its name in the UI font and `Aa 012` in the
candidate family, with a selected marker and check. A applies the family and
rebuilds the shell.

### Music

An 84 px album card, vinyl disc, title, artist, format, progress bar, elapsed
and total time, and three transport buttons. Play is 44 px; previous/next are
30 px with **6 px gaps** (no overlapping hit areas). X/Y or L/R changes track
without moving focus. A 700 ms timer updates the
position only while the screen exists.

### System

Four 102 × 54 metric cards: CPU, memory, battery and uptime, each with a label,
large value and note. A back button and build footer complete the screen.

### About

Centered orb, wordmark, codename/version, mission statement and a two-line
component credit. Back is the only control.

## Focus and controls

The RG Nano mapping is:

| Button | Action |
|---|---|
| D-pad | move focus spatially |
| A | open / activate / cycle |
| B or Select | back |
| Start | Home |
| X / Y or L / R | previous/next track, preview theme, adjust wallpaper or slider |

Every interactive object is clickable, joins the current `lv_group` and gets a
2 px accent focus ring. The shell maintains a **back stack** for nested screens:
Settings → Appearance → Themes → B returns to Appearance, then Settings, then
Home. Changing tabs clears the stack.

## Motion

- Screen entry: 200 ms fade + 10 px slide.
- Staggered content reveal: 240–300 ms.
- Wallpapers: slow ping-pong translations, opacity breathing and ring scaling.
- Speed: Still, Slow, Normal, Fast. **Reduce motion** forces Still while keeping
  the chosen speed for later.
- Animations are stopped when their objects are deleted; no global animation
  timer and no full-screen effects.

## Contrast and legibility

- Primary text: at least 4.5:1 on every surface; most palettes exceed 7:1.
- Metadata and dim hints: at least 4.5:1.
- Accent icons and text: at least 4.5:1.
- Destructive red: at least 4.5:1.
- No text directly on a moving wallpaper; cards use opaque or near-opaque
  surfaces.
- Status time, brand and battery are split into separate labels so a typeface
  without symbol glyphs cannot hide battery state.

## Performance rules

- Native 240 × 240 rendering; partial refresh only.
- Static font bitmaps, no runtime font loading or rasterisation.
- One wallpaper layer behind status/content/navigation; child shapes are clipped.
- The wallpaper uses a small, fixed number of LVGL objects and stores its
  animation handles so they can be stopped explicitly on deletion.
- One status timer and at most one module timer; both die with the screen.
- No blur, no full-screen shadow stacks, no runtime image decoding for chrome.
