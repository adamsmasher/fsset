#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

void debug(const char *format, ...) {
#ifdef DEBUG
    va_list args;

    va_start(args, format);

    fprintf(stderr, "DEBUG: ");
    vfprintf(stderr, format, args);

    va_end(args);
#endif
}


