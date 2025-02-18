#ifndef LOGGER_H
#define LOGGER_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} LogLevel;

extern void rw_log(LogLevel level, const char* format, ...);

#endif // LOGGER_H
