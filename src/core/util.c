#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define TINYOS_MKDIR(path) _mkdir(path)
#define TINYOS_FSYNC(fd) _commit(fd)
#define TINYOS_UNLINK(path) _unlink(path)
#define TINYOS_OPEN_FLAGS (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY)
#else
#define TINYOS_MKDIR(path) mkdir(path, 0755)
#define TINYOS_FSYNC(fd) fsync(fd)
#define TINYOS_UNLINK(path) unlink(path)
#define TINYOS_OPEN_FLAGS (O_WRONLY | O_CREAT | O_TRUNC)
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

bool tinyos_mkdir_p(const char *path)
{
    if (!path || !*path)
        return false;

    char buffer[PATH_MAX];
    size_t len = strlen(path);
    if (len >= sizeof(buffer))
        return false;
    memcpy(buffer, path, len + 1);

    for (char *p = buffer + 1; *p; p++) {
        if (*p != '/')
            continue;
        *p = '\0';
        if (TINYOS_MKDIR(buffer) != 0 && errno != EEXIST)
            return false;
        *p = '/';
    }

    if (TINYOS_MKDIR(buffer) != 0 && errno != EEXIST)
        return false;
    return true;
}

int tinyos_read_file(const char *path, char *buf, size_t size)
{
    if (!path || !buf || size == 0)
        return -1;

    FILE *file = fopen(path, "rb");
    if (!file)
        return -1;

    size_t read = fread(buf, 1, size - 1, file);
    int error = ferror(file);
    fclose(file);
    if (error)
        return -1;

    buf[read] = '\0';
    return (int)read;
}

int tinyos_write_file_atomic(const char *path, const char *data)
{
    if (!path || !data)
        return -1;

    char tmp[PATH_MAX];
    if (snprintf(tmp, sizeof(tmp), "%s.tmp", path) >= (int)sizeof(tmp))
        return -1;

    int fd = open(tmp, TINYOS_OPEN_FLAGS, 0644);
    if (fd < 0)
        return -1;

    size_t left = strlen(data);
    const char *cursor = data;
    while (left > 0) {
        ssize_t written = write(fd, cursor, left);
        if (written < 0) {
            if (errno == EINTR)
                continue;
            close(fd);
            TINYOS_UNLINK(tmp);
            return -1;
        }
        cursor += written;
        left -= (size_t)written;
    }

    if (TINYOS_FSYNC(fd) != 0) {
        close(fd);
        TINYOS_UNLINK(tmp);
        return -1;
    }
    close(fd);

    if (rename(tmp, path) != 0) {
        TINYOS_UNLINK(tmp);
        return -1;
    }
    return 0;
}

char *tinyos_trim(char *s)
{
    if (!s)
        return s;

    while (isspace((unsigned char)*s))
        s++;

    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1]))
        *--end = '\0';
    return s;
}

int tinyos_clamp_int(int value, int minimum, int maximum)
{
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}
