#include <stdio.h>
#include <stdarg.h>
#include "colors.h"
#include "debug_aux.h"

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
    va_list ap;

    printf("DEBUGGING");
    switch (type)
    {
    case DEBUG_OK:
        printf(" [");
        setcolor(COLOR_GREEN);
        printf("OK");
        setcolor(COLOR_RESET);
        printf("]: ");
        break;
    case DEBUG_INFO:
        printf(" [");
        setcolor(COLOR_CYAN);
        printf("INFO");
        setcolor(COLOR_RESET);
        printf("]: ");
        break;
    case DEBUG_WARN:
        printf(" [");
        setcolor(COLOR_YELLOW);
        printf("WARING");
        setcolor(COLOR_RESET);
        printf("]: ");
        break;
    case DEBUG_ERROR:
        printf(" [");
        setcolor(COLOR_RED);
        printf("ERROR");
        setcolor(COLOR_RESET);
        printf("]: ");
        break;
    case DEBUG_PROCESSING:
        printf(" [");
        setcolor(COLOR_BLUE);
        printf("PROCESSING");
        setcolor(COLOR_RESET);
        printf("]: ");
        break;
    case DEBUG_WAITING:
        printf(" [");
        setcolor(COLOR_MAGENTA);
        printf("WAITING");
        setcolor(COLOR_RESET);
        printf("]: ");
        break;

    default:
        printf(" [OTHER]: ");
        break;
    }

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    printf("\n");
#endif
}