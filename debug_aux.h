#ifndef _DEBUG_AUX_H_
#define _DEBUG_AUX_H_

void on_debug(int type, char *fmt, ...);

#define DEBUG_OK 1
#define DEBUG_INFO 2
#define DEBUG_WARN 3
#define DEBUG_ERROR 4
#define DEBUG_PROCESSING 5
#define DEBUG_WAITING 6

#define ON_DEBUG(type, ...) on_debug((type), __VA_ARGS__)

#endif