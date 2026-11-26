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
| 28 | splash orb |
| 24 | about wordmark |
| 20 | screen titles, greeting |
| 16 | player title, metric values |
| 14 | status time and battery percentage, empty-state title, dense numerals |
| 12 | row titles, card titles |
| 10 | labels, values, hints, metadata |

No text is smaller than 10 px; 10 px is reserved for supporting copy that is
never the only way to identify a control. Long labels use `LV_LABEL_LONG_MODE_DOTS`; only
deliberate multi-line copy opts into wrapping. Typeface changes rebuild the
shell and are stored in `/data/tinyos/settings.ini`.

## Screen patterns

### Home

A 212 × 88 hero card with the date, greeting, one-line tagline and a gradient
orb. Three 64 × 44 quick tiles (Music, System, About) sit below a
“QUICK ACCESS” eyebrow — only destinations the tab bar cannot reach. The wallpaper remains visible around the cards but
never behind body text without a solid surface.

### Library

A 2 × 2 grid of 102 × 50 app tiles at y = 42 and y = 100 (8 px gutter) with a
24 px badge, 12 px title and 10 px subtitle. The focused tile gets a 2 px
accent ring. Tiles carry no chevrons; the whole card is the target.

### Settings

Six compact 22 px rows with 2 px gaps and a 9 px radius, starting at y = 28:
**Appearance**, Brightness, Volume, 24-hour clock, Performance and Power off.
About lives in the Library and Home quick access, so Settings does not repeat
it. Sliders, switches and pills share a 198 px right edge; slider tracks are
88 px wide at x = 110. Sliders and the clock switch focus their whole row, so
the focus ring never crosses a knob. The theme pill uses the compact theme name
(for example “Mono” for Monochrome) and never ellipsises. Power off is red and
opens a 180 × 108 confirmation overlay at (30, 62) with CANCEL and POWER OFF;
the overlay takes the group focus. Power off is red
and opens a confirmation overlay. Appearance is always the first row so
personalisation is discoverable.

### Appearance

Four 24 px rows with 2 px gaps: Theme, Wallpaper, Motion and Typeface. Each
row shows its current value in a 72 × 18 pill and a 12 px chevron at x = 186.
Theme and typeface pills use the compact names (“Mono”, “Grotesk”).
A back button sits at the top left; A opens the picker, B returns to Settings.

### Themes

A live 212 × 62 preview card at y = 42, palette name, `NN / 25` counter and a
hint line. X/Y or L/R steps through the gallery; A applies immediately and
rebuilds the UI. The preview never uses the old palette for its sample cards.

### Wallpapers

A 212 × 54 animated preview at y = 42 above two 22 px rows at y = 100 and
y = 124: Wallpaper and Speed, each with a 72 × 18 pill at x = 106. The hint
line sits at y = 152. Motion lives in Appearance. Style and speed cycle on A;
X/Y or L/R adjusts the focused row. Changes are saved and applied to the shell
wallpaper immediately, so the preview and the real background never drift
apart.

### Typeface

Six 21 px rows. Each row renders its name at 12 px and `Aa 012` in the
candidate family at 12 px, with a selected marker and check. A applies the family and
rebuilds the shell.

### Music

A 60 px album card at (14, 34), vinyl disc, 16 px title at (86, 38), 10 px
artist at y = 63 and 10 px format at y = 78, progress bar at y = 102, elapsed
and total time, and three transport buttons. The title is one line in
`LV_LABEL_LONG_MODE_DOTS`, so only an unusually long file name ends in an
ellipsis; all fixed interface labels fit without one. Play is 44 px with a
flat fill; previous/next are 28 px with **6 px gaps** (no overlapping hit
areas). X/Y or
L/R changes track without moving focus. The empty state is a 212 × 96 card at
y = 44 with a centered 40 px note, “No tracks yet”, a 10 px path hint and an
84 × 20 RESCAN pill; its content keeps 7 px bottom padding. A 700 ms timer
updates the position only while the screen exists.

### System

Four 102 × 56 metric cards: CPU, memory, battery and uptime, each with a 10 px
label, 16 px value and 10 px note. A back button completes the screen.

### About

Centered 42 px orb, 24 px wordmark, codename/version, mission statement and a
two-line component credit. Back is the only control.

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
2 px accent focus ring. Rows that contain a single control (Appearance,
sliders, clock, Performance, media transport, navigation) take the ring, so it
is never clipped by a knob or a background element. A 90 ms style transition
fades the ring and the pressed fill; nothing moves.

- A scrollable list scrolls the newly focused row into view.
- The power-off dialog owns a **separate group**: the D-pad cannot reach the
  screen behind it, B closes the dialog and focus returns to the Power off row.
  Start closes the dialog before going Home.
- The selected tab stays lit on detail screens: Settings stays active on
  Appearance, Themes, Wallpapers and Typeface, Library stays active on Music,
  System and About. Selected (filled pill) and focused (outline) are distinct
  treatments. The shell maintains a **back stack** for nested screens:
Settings → Appearance → Themes → B returns to Appearance, then Settings, then
Home. Changing tabs clears the stack.

## Motion

Motion confirms a state change; it never delays input or moves the layout.

| Event | Duration | Easing |
|---|---|---|
| Focus / press ring and fill | 90 ms | ease-out |
| Screen entry | 200 ms fade + 10 px slide | ease-out |
| Card reveal | 260–300 ms, 40 ms stagger | ease-out |
| Dialog entry | 150 ms fade + 8 px rise | ease-out |
| Theme step (inner samples) | 140 ms, 5 px settle | ease-out |
| Slider value | 90 ms | ease-out |
| Music progress | 450 ms | bar animation |

- Focus changes cross-fade the ring colour; the ring never travels across the
  screen and never bounces.
- Pressed is a separate state: a 2 px primary-colour ring over the focused ring,
  so a held A never hides where the user is.
- The dialog slides and fades in as one 180 × 108 card; the backdrop itself is
  static, avoiding a full-screen redraw.
- The palette preview keeps its card on the grid; only the two samples inside
  move 5 px when stepping.
- Wallpapers: slow ping-pong translations, opacity breathing and ring scaling.
- Speed: Still, Slow, Normal, Fast. **Reduce motion** forces Still and disables
  content reveals, the dialog rise, nudges, slider and progress animation; focus
  colours still change, just without movement.
- Animations are stopped when their objects are deleted; no global animation
  timer and no full-screen effects.

## Contrast and legibility

- Primary text: at least 4.5:1 on every surface; most palettes exceed 7:1.
- Metadata and dim hints: at least 4.5:1.
- Accent icons and text: at least 4.5:1.
- Destructive red: at least 4.5:1.
- No text directly on a moving wallpaper; cards use opaque or near-opaque
  surfaces.
- The status line carries a 14 px time, a 14 px battery icon at (162, 6) and a
  14 px percentage, split into separate labels so a typeface without symbol
  glyphs cannot hide battery state. There is no second wordmark.

## Performance rules

- Native 240 × 240 rendering; partial refresh only; `LV_DEF_REFR_PERIOD` is
  16 ms (60 Hz) with partial draw buffers.
- Static font bitmaps, no runtime font loading or rasterisation.
- No large shadows on moving controls; the transport play button is a flat
  accent fill.
- Host-simulator check, 300 frames per run: 35 ms with Aurora, 60 ms with
  Ripple, 19 ms with Reduce Motion (relative host cost, not device FPS).
- One wallpaper layer behind status/content/navigation; child shapes are clipped.
- The wallpaper uses a small, fixed number of LVGL objects and stores its
  animation handles so they can be stopped explicitly on deletion.
- One status timer and at most one module timer; both die with the screen.
- No blur, no full-screen shadow stacks, no runtime image decoding for chrome.
