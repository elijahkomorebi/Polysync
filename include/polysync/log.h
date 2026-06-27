#ifndef POLYSYNC_LOG_H
#define POLYSYNC_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "polysync/types.h"

void polysync_log_generic(FILE *output, cstring *format, va_list *arguments, cstring *prefix, cstring *color);
void polysync_log_info(cstring *format, ...);
void polysync_log_warning(cstring *format, ...);
void polysync_log_error(cstring *format, ...);

#endif
