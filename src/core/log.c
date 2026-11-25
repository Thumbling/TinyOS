#include "log.h"

#include <stdarg.h>
#include <stdio.h>

void tinyos_log(const char *level, const char *format, ...)
{
    va_list args;
    fprintf(stderr, "[tinyos:%s] ", level ? level : "log");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
    fflush(stderr);
}
