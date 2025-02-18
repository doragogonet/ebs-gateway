#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} LogLevel;

extern void rw_log(LogLevel level, const char* format, ...);
extern int rw_init(const char* config_file);

#endif // LOGGER_H
