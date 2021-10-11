#ifndef _MESSAGE_H_
#define _MESSAGE_H_

void on_debug(int type, char *fmt, ...);
void on_message(int type, char *fmt, ...);
void on_error(int err, char *fmt, ...);

#define MESSAGE_OK 1
#define MESSAGE_INFO 2
#define MESSAGE_WARN 3
#define MESSAGE_ERROR 4
#define MESSAGE_PROCESSING 5
#define MESSAGE_WAITING 6
#define MESSAGE_MISMATCH 7
#define MESSAGE_SUMMARY 8

/**
 * @see on_debug() 
 * 
 */
#define ON_DEBUG(type, ...) on_debug((type), __VA_ARGS__)

#define MESSAGE(type, ...) on_message((type), __VA_ARGS__)

#define MSG_FILE_TOOLONG(arg) MESSAGE(MESSAGE_ERROR, "'%s': file path too long - %s", (arg), strerror(ENAMETOOLONG))
#define MSG_FILE_NOT_EXISTS(file) MESSAGE(MESSAGE_ERROR, "cannot open file '%s' - %s", (file), strerror(ENOENT))
#define MSG_INCORRECT_FILE_ARG(file) MESSAGE(MESSAGE_ERROR, "file format not suitable for this '%s' - %s", (file), strerror(EINVAL))

#define ERROR_CANT_OPEN_FILE(file) on_error((errno), "cannot open file '%s'", (file))
#define ERROR_FILE_NOT_EXISTS(file) on_error((ENOENT), "cannot open file '%s'", (file))
#define ERROR_INCORRECT_FILE_ARG(file) on_error((EINVAL), "file format not suitable for this '%s'", (file))
#define ERROR_CANT_OPEN_DIR(dir) on_error((errno), "cannot open dir '%s'", (dir))
#define ERROR_CANT_START_PROC(par_pid) on_error((errno), "unable to start new process on %d", (par_pid))

#endif