#include <stdio.h>
#include <stdarg.h>
#include "debug_aux.h"

void on_debug(int type, char *fmt, ...)
{
    /* Removing unused variable error */
    (void)type;
    (void)fmt;

#ifdef SHOW_DEBUG
    va_list ap;

    printf("DEBUGGING");
    switch (type)
    {
    case DEBUG_OK:
        printf(" [OK]: ");
        break;
    case DEBUG_INFO:
        printf(" [INFO]: ");
        break;
    case DEBUG_WARN:
        printf(" [WARNING]: ");
        break;
    case DEBUG_ERROR:
        printf(" [ERROR]: ");
        break;

    default:
        printf(" [CUSTOM]: ");
        break;
    }

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    printf("\n");
#endif
}