#include "error.h"

#include <stdarg.h>
#include <stdio.h>

int error(const char *format, ...) {
    va_list args;

    va_start(args, format);

    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, format, args);

    va_end(args);

    return ERROR;
}

