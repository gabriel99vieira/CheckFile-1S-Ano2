#ifndef _COLORS_H_
#define _COLORS_H_

/**
 * 
 * Syntax provided from:
 * https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix/3586005#3586005
 * 
 */

#define COLOR_BLACK "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_WHITE "\x1B[37m"
#define COLOR_RESET "\033[0m"

void setcolor(const char *color);

#endif