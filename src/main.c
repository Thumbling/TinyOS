#include "app.h"

#include <signal.h>
#include <stddef.h>

static void handle_signal(int signal_number)
{
    (void)signal_number;
    tinyos_ctx *ctx = tinyos_ctx_get();
    if (ctx)
        ctx->running = false;
}

int main(void)
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    return tinyos_app_run();
}
