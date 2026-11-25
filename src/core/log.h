#ifndef TINYOS_CORE_LOG_H
#define TINYOS_CORE_LOG_H

void tinyos_log(const char *level, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

#define TINYOS_LOGI(...) tinyos_log("info", __VA_ARGS__)
#define TINYOS_LOGW(...) tinyos_log("warn", __VA_ARGS__)
#define TINYOS_LOGE(...) tinyos_log("error", __VA_ARGS__)

#endif
