#ifndef POLYSYNC_ERROR_H
#define POLYSYNC_ERROR_H

#include "polysync/log.h"
#include "polysync/exit.h"

#define polysync_error(format, ...) \
    do { \
        polysync_log_error(format, ##__VA_ARGS__); \
        polysync_exit(POLYSYNC_EXIT_FAILURE); \
    } while (0)

#endif
