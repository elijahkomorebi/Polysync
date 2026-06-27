#include "polysync/log.h"

static cstring *LOG_PREFIX_INFO = "[Info] ";
static cstring *LOG_PREFIX_WARNING = "[Warning] ";
static cstring *LOG_PREFIX_ERROR = "[Error] ";
static cstring *LOG_COLOR_RESET = "\x1b[0m";
static cstring *LOG_COLOR_BLUE = "\x1b[34m";
static cstring *LOG_COLOR_YELLOW = "\x1b[33m";
static cstring *LOG_COLOR_RED = "\x1b[31m";

void polysync_log_generic(FILE *output, cstring *format, va_list *arguments, cstring *prefix, cstring *color) {
    fprintf(output, "%s%s", color, prefix);
    vfprintf(output, format, *arguments);
    fprintf(output, "%s\n", LOG_COLOR_RESET);
}

void polysync_log_info(cstring *format, ...) {
    va_list arguments;

    va_start(arguments, format);
    polysync_log_generic(stdout, format, &arguments, LOG_PREFIX_INFO, LOG_COLOR_BLUE);
    va_end(arguments);
}

void polysync_log_warning(cstring *format, ...) {
    va_list arguments;

    va_start(arguments, format);
    polysync_log_generic(stdout, format, &arguments, LOG_PREFIX_WARNING, LOG_COLOR_YELLOW);
    va_end(arguments);
}

void polysync_log_error(cstring *format, ...) {
    va_list arguments;

    va_start(arguments, format);
    polysync_log_generic(stderr, format, &arguments, LOG_PREFIX_ERROR, LOG_COLOR_RED);
    va_end(arguments);
}
