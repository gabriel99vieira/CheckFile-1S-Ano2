#include <stdio.h>
#include <stdarg.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include "colors.h"
#include "message.h"

/**
 * @brief A display message macro to make ther terminal more appealing
 *
 * @param type
 * @param fmt
 * @param ...
 */
void on_message(int type, char *fmt, ...)
{
    /* Removing unused variable error */
    (void)type;
    (void)fmt;

    va_list ap;

    // printf("MESSAGE");
    printf("\n[");
    switch (type)
    {
    case MESSAGE_OK:
        setcolor(COLOR_GREEN);
        printf("OK");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_INFO:
        setcolor(COLOR_CYAN);
        printf("INFO");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_WARN:
        setcolor(COLOR_YELLOW);
        printf("WARN");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_ERROR:
        setcolor(COLOR_RED);
        printf("ERROR");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_PROCESSING:
        setcolor(COLOR_BLUE);
        printf("PROCESS");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_WAITING:
        setcolor(COLOR_MAGENTA);
        printf("WAITING");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_MISMATCH:
        setcolor(COLOR_MAGENTA);
        printf("MISMATCH");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_SUMMARY:
        setcolor(COLOR_YELLOW);
        printf("SUMMARY");
        setcolor(COLOR_RESET);
        break;
    case MESSAGE_SIGNAL:
        setcolor(COLOR_CYAN);
        printf("SIGNAL");
        setcolor(COLOR_RESET);
        break;

    default:
        printf("OTHER");
        break;
    }
    printf("]: ");

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

/**
 * @brief A display message only when SHOW_DEBUG flag is set
 *
 * @param type
 * @param fmt
 * @param ...
 */
void on_debug(int type, char *fmt, ...)
{
    /* Removing unused variable error */
    (void)type;
    (void)fmt;

#ifdef SHOW_DEBUG
    on_message(type, fmt);
#endif
}

void on_error(int err, int extCode, char *fmt, ...)
{
    /* Removing unused variable error */
    (void)fmt;

    errno = err;

    va_list ap;

    printf("\n[");
    setcolor(COLOR_RED);
    printf("ERROR");
    setcolor(COLOR_RESET);
    printf("]: ");

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    printf(" - %s\n", strerror(errno));
    exit(extCode);
}
