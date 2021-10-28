#ifndef _MESSAGE_H_
#define _MESSAGE_H_

void on_debug(int type, char *fmt, ...);
void on_message(int type, char *fmt, ...);
void on_error(int err, int extCode, char *fmt, ...);

#define MESSAGE_OK 1
#define MESSAGE_INFO 2
#define MESSAGE_WARN 3
#define MESSAGE_ERROR 4
#define MESSAGE_PROCESSING 5
#define MESSAGE_WAITING 6
#define MESSAGE_MISMATCH 7
#define MESSAGE_SUMMARY 8
#define MESSAGE_SIGNAL 9

#define C_ERROR_FILE_NOT_EXISTS 1
#define C_ERROR_INCRRECT_OR_INVALID_ARG 2
#define C_ERROR_UNABLE_OPEN_FILE 3
#define C_ERROR_UNABLE_OPEN_DIR 4
#define C_ERROR_UNABLE_START_PROC 5
#define C_ERROR_UNABLE_EXECUTE_PROC 6
#define C_ERROR_UNABLE_TO_SET_SIGNAL_HANDLER 7
#define C_ERROR_UNABLE_WRITE_FILE 8

/**
 * @see on_debug()
 *
 */
#define ON_DEBUG(type, ...) on_debug((type), __VA_ARGS__)
#define MESSAGE(type, ...) on_message((type), __VA_ARGS__)
#define ON_ERROR(extCode, ...) on_error((errno), (extCode), __VA_ARGS__)

#define MSG_FILE_TOOLONG(arg) MESSAGE(MESSAGE_ERROR, "'%s': file path too long - %s", (arg), strerror(ENAMETOOLONG))
#define MSG_FILE_NOT_EXISTS(file) MESSAGE(MESSAGE_ERROR, "cannot open file '%s' - %s", (file), strerror(errno))
#define MSG_INCORRECT_FILE_ARG(file) MESSAGE(MESSAGE_ERROR, "file format not suitable for this '%s' - %s", (file), strerror(EINVAL))

#define ERROR_CANT_WRITE_FILE(file) on_error((errno), (C_ERROR_UNABLE_WRITE_FILE), "cannot write to file '%s'", (file))
#define ERROR_CANT_OPEN_FILE(file) on_error((errno), (C_ERROR_UNABLE_OPEN_FILE), "cannot open file '%s'", (file))
#define ERROR_FILE_NOT_EXISTS(file) on_error((errno), (C_ERROR_FILE_NOT_EXISTS), "cannot open file '%s'", (file))
#define ERROR_INCORRECT_FILE_ARG(file) on_error((EINVAL), (C_ERROR_INCRRECT_OR_INVALID_ARG), "file format not suitable for this '%s'", (file))
#define ERROR_CANT_OPEN_DIR(dir) on_error((errno), (C_ERROR_UNABLE_OPEN_DIR), "cannot open directory '%s'", (dir))
#define ERROR_CANT_CLOSE_DIR(dir) on_error((errno), (C_ERROR_UNABLE_OPEN_DIR), "cannot close directory '%s'", (dir))
#define ERROR_CANT_START_PROC() on_error((errno), (C_ERROR_UNABLE_START_PROC), "unable to start new process")
#define ERROR_CANT_EXECUTE_PROC() on_error((errno), (C_ERROR_UNABLE_EXECUTE_PROC), "unable to execute process")
#define ERROR_UNABLE_SET_SIGNAL_HANDLER(sig) on_error((errno), (C_ERROR_UNABLE_TO_SET_SIGNAL_HANDLER), "unable to set signal handler for '%s'", strsignal((sig)))

#endif