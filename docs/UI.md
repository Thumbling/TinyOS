# UI and design system

## Intent

TinyOS should feel like a small, deliberate device — not a desktop shrunk onto
a 240 px screen. The UI uses a dark blue-black canvas, one strong accent per
context, large type and generous negative space. Every screen has a single
obvious focus and a maximum of one primary action.

## Canvas

| Property | Value |
|---|---|
| Resolution | 240 × 240 |
| Status line | y = 0…26 |
| Content | y = 30…204 |
| Navigation | y = 207…235 |
| Safe margin | 14 px left/right |
| Corner radius | 12–20 px |
| Border | 1 px, low-opacity line colour |

## Colour tokens

| Token | Hex | Use |
|---|---|---|
| `BG` | `#070B10` | deepest background |
| `BG_2` | `#0B1520` | background gradient top |
| `SURFACE` | `#101B25` | cards and rows |
| `SURFACE_2` | `#162632` | raised surfaces |
| `LINE` | `#1D3140` | hairlines and borders |
| `TEXT` | `#F2F8FB` | primary text |
| `MUTED` | `#8DA3B3` | secondary text |
| `DIM` | `#5B7180` | metadata |
| `CYAN` | `#3DE1FF` | primary accent |
| `CYAN_DEEP` | `#0C6E86` | gradient partner |
| `VIOLET` | `#9A86FF` | media/library accent |
| `GREEN` | `#48E6A0` | system/positive |
| `AMBER` | `#FFB454` | settings/about accent |
| `RED` | `#FF7070` | destructive actions |

Accents are used for icons, indicators, values and one primary action — not as
large flat fills. This keeps the RG Nano panel, which has limited brightness,
from looking like a toy.

## Typography

Montserrat, bundled with LVGL:

| Size | Use |
|---|---|
| 28 | home wordmark, about wordmark |
| 24 | hero orb letter, about title |
| 20 | screen titles |
| 16 | system values, nav icons |
| 14 | body, status time |
| 10 | labels, secondary values |
| 8 | eyebrows, metadata, status brand |

Text is never smaller than 8 px. Long labels use `LV_LABEL_LONG_MODE_DOTS`; only
intentional multi-line copy opts into wrapping.

## Screen patterns

### Home

A 212 × 94 hero card with a greeting eyebrow, the wordmark, a two-line tagline,
a cyan “OPEN LIBRARY” pill and a gradient orb. Three quick tiles (Music,
Library, Settings) sit below a “QUICK ACCESS” eyebrow. The hero is the visual
anchor; the quick tiles are neutral so the accents read as navigation, not
decoration.

### Library

A large screen title with the subtitle “Everything in one place.” and a 2 × 2
grid of app tiles. Each tile has a 36 px rounded icon badge, a title and a short
subtitle. The focused tile gets a 2 px cyan ring. Footer copy “Modular by
design.” reinforces the architecture without adding another control.

### Settings

Six compact rows: Brightness, Volume, 24-hour clock, Performance, About TinyOS
and Power off. Sliders and switches live on the row, so the user never leaves
the list to change a value. Power off is red and opens a confirmation dialog.
Rows are 22 px high with 2 px gaps; all six fit without scrolling.

### Music

A 84 px album card with a gradient and a vinyl-style disc, track title, artist,
format, progress bar, elapsed/total time and three transport buttons. The play
button is 44 px and cyan; previous/next are 30 px neutral buttons. A timer
refreshes position every 700 ms while the screen is alive.

### System

Four metric cards: CPU MHz, memory, battery and uptime. Each card has an
eyebrow label, a large value and a small note. A back button and a footer with
the build identity complete the screen.

### About

A centered orb, wordmark, codename/version, a short mission statement and a
two-line component credit. This is the “why” screen; it has no controls beyond
Back.

## Focus

Every interactive object gets:

- `LV_OBJ_FLAG_CLICKABLE`;
- a 2 px cyan border in `LV_STATE_FOCUSED`;
- a clear shape that remains legible at 240 × 240;
- membership in the current `lv_group`.

Focus order is intentional: the first module focusable is the primary action,
then secondary actions, then Back. The D-pad moves spatially through the group;
A activates; B goes back.

## Motion

- Screen transitions: 200 ms fade + 10 px horizontal slide.
- Content reveals: 260–300 ms fade + 12 px vertical slide, staggered by 35 ms.
- No infinite loops, parallax or full-screen blurs.
- `reduce_motion` is a stored setting; modules should honour it when adding new
  animations.

The transitions are short enough to feel responsive on the V3s and are disabled
by the small draw budget rather than by taste alone.

## Contrast and legibility

- Primary text on background: > 12:1.
- Muted text: > 5:1.
- Accent icons on dark surfaces: > 7:1.
- Focus ring: cyan on dark, 2 px.
- No text over gradients without a solid card behind it.
- Battery/time remain visible on every screen.

## Performance rules

- Partial rendering only; no full-screen animation buffers.
- Reuse fonts; no runtime font loading.
- Use solid cards and simple gradients; avoid shadows on large objects.
- Keep the LVGL heap at 256 KB and watch `lv_mem_monitor` when adding screens.
- Prefer one timer per screen, deleted with the screen.

## Future visual work

- A boot splash that matches the orb/wordmark.
- A theme picker (accent variants) using the existing tokens.
- Album art extraction from WAV metadata or a sidecar file.
- A compact now-playing overlay accessible from any tab.

These are intentionally not implemented yet; the current UI is the baseline,
not a mockup.
