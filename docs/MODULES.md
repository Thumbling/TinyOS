# Modules

TinyOS is a shell plus a table of modules. A module owns one screen, builds its
widgets under the content viewport and registers the focusables it wants the
D-pad to reach.

## Contract

```c
typedef struct tinyos_module {
    const char *id;       /* stable id used by tinyos_shell_show() */
    const char *name;     /* human-readable name */
    const char *subtitle; /* short description for cards */
    const char *icon;     /* LV_SYMBOL_* */
    lv_obj_t *(*create)(tinyos_ctx *ctx, lv_obj_t *parent);
} tinyos_module;
```

`create()` must:

- build only inside `parent`;
- call `tinyos_shell_add_focus(ctx, obj)` for every focusable;
- attach an `LV_EVENT_DELETE` callback to `parent` if it allocates heap state or
  timers;
- return `parent` (the return value is currently informational).

The shell deletes every child of the content viewport on navigation, so a
module must not keep pointers to deleted widgets. Use a per-screen view struct
allocated in `create()` and freed in the delete callback, as `music.c` does.

## Current modules

| id | Screen | Focus starts on | Notes |
|---|---|---|---|
| `home` | Home | hero CTA | greeting, orb, quick tiles |
| `apps` | Library | Music tile | four app tiles |
| `settings` | Settings | brightness | sliders, switches, confirm |
| `music` | Music | play | WAV player, rescan, timer |
| `system` | System | back | live CPU, memory, battery, uptime |
| `about` | About | back | identity and component credits |

## Adding a module

1. Create `src/modules/example.c` with a `create()` function and a descriptor.
2. Declare it in `src/modules/module.h`.
3. Add the descriptor to the table in `src/modules/modules.c`.
4. Add the source file to `SOURCES` in `src/Makefile`.
5. Add a tile in `src/modules/apps.c` (or a quick tile on Home) that calls
   `tinyos_shell_show(ctx, "example")`.
6. Build `make sim` and check focus order with the simulator.

Minimal skeleton:

```c
#include "module.h"
#include "../ui/shell.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"

static lv_obj_t *example_create(tinyos_ctx *ctx, lv_obj_t *parent)
{
    tinyos_label(parent, "Example", &lv_font_montserrat_20,
                 TINYOS_COLOR_TEXT, 14, 0, 212, 24);

    lv_obj_t *action = tinyos_obj(parent, 14, 40, 212, 32);
    tinyos_card(action, 14, 40, 212, 32);
    tinyos_focus(action);
    tinyos_shell_add_focus(ctx, action);

    return parent;
}

const tinyos_module tinyos_module_example = {
    .id = "example",
    .name = "Example",
    .subtitle = "A new module",
    .icon = LV_SYMBOL_PLUS,
    .create = example_create,
};
```

## Focus and navigation

- The shell creates a fresh `lv_group` on every screen change.
- Nav buttons are added first; module focusables follow.
- The first module focusable receives focus. Call `tinyos_shell_add_focus()` in
  the order you want the user to move.
- `LV_KEY_ESC` invokes the shell back action; `LV_KEY_HOME` jumps Home.
- The back button is a normal focusable; include it in the group.

## State and cleanup

Use `calloc()` for screen state and attach:

```c
lv_obj_add_event_cb(parent, delete_cb, LV_EVENT_DELETE, view);
```

Delete timers and free state in `delete_cb`. Do not persist LVGL object pointers
beyond the screen lifetime. Persistent data belongs in `/data/tinyos/` via the
settings or audio APIs.

## Styling

Use the shared theme tokens and widgets (`tinyos_card`, `tinyos_label`,
`tinyos_icon`, `tinyos_focus`, `tinyos_reveal`). Do not hard-code a second
visual language. If a new token is genuinely needed, add it to
`src/ui/theme.h` and document it in [UI.md](UI.md).

## Testing a module

```sh
make sim
./build/tinyos-sim --screen example --out build/example.png
```

The simulator creates the module in the same shell and renders it at 240 × 240.
Check label overflow, focus ring clipping and the nav overlap before adding the
module to the real image.
