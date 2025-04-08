
#include "logger.h"

void rw_log(LogLevel level, const char* format, ...);
void rw_log(LogLevel level, const char* format, ...) {

    const char* level_str[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    char buffer[1024];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    snprintf(buffer + 19, 1024 - 19, " [%s] ", level_str[level]);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer + strlen(buffer), 1024 - strlen(buffer), format, args);
    va_end(args);

    printf("%s\n", buffer);
}

