#ifndef TINYOS_CORE_UTIL_H
#define TINYOS_CORE_UTIL_H

#include <stdbool.h>
#include <stddef.h>

bool tinyos_mkdir_p(const char *path);
int tinyos_read_file(const char *path, char *buf, size_t size);
int tinyos_write_file_atomic(const char *path, const char *data);
char *tinyos_trim(char *s);
int tinyos_clamp_int(int value, int minimum, int maximum);

#endif
