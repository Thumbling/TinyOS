#ifndef TINYOS_UI_SHELL_H
#define TINYOS_UI_SHELL_H

#include "../app.h"

void tinyos_shell_init(tinyos_ctx *ctx);
void tinyos_shell_show(tinyos_ctx *ctx, const char *id);
void tinyos_shell_back(tinyos_ctx *ctx);
void tinyos_shell_reload(tinyos_ctx *ctx, const char *id);
void tinyos_shell_set_wallpaper(tinyos_ctx *ctx, int style, int speed);
void tinyos_shell_refresh_status(tinyos_ctx *ctx);
void tinyos_shell_add_focus(tinyos_ctx *ctx, lv_obj_t *obj);

#endif
