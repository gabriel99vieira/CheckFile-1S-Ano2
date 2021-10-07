#ifndef _MESSAGE_H_
#define _MESSAGE_H_

void on_debug(int type, char *fmt, ...);

#define DEBUG_OK 1
#define DEBUG_INFO 2
#define DEBUG_WARN 3
#define DEBUG_ERROR 4
#define DEBUG_PROCESSING 5
#define DEBUG_WAITING 6

#define MESSAGE_OK 1
#define MESSAGE_INFO 2
#define MESSAGE_WARN 3
#define MESSAGE_ERROR 4
#define MESSAGE_PROCESSING 5
#define MESSAGE_WAITING 6

/**
 * @see on_debug() 
 * 
 */
#define ON_DEBUG(type, ...) on_debug((type), __VA_ARGS__)

#define MESSAGE(type, ...) on_message((type), __VA_ARGS__)

#endif