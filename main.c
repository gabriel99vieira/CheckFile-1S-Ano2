/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
// #include <sys/wait.h>
// #include <sys/sysmacros.h>

#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
// #include <dirent.h>
// #include <time.h>
// #include <assert.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "message.h"

/**
 * Application variables
 */
#define MAX_QUEUE 100
#define MAX_FILE_PATH 100

/**
 * Application errors
 */
#define ARGUMENT_MISSING 10
#define ARGUMENT_INCORRECT 11
#define EMPTY_FILE 12

struct gengetopt_args_info args_info;

off_t file_size(int fd)
{
    int offset = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);
    return offset;
}

int main(int argc, char *argv[])
{
    //* Disable warnings */
    (void)argc;
    (void)argv;

    ON_DEBUG(DEBUG_INFO, "Gengetopt validation");
    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(0);
    }

    char queue_files[MAX_QUEUE][MAX_FILE_PATH];
    int queue_counter = 0;
    struct stat f_stat;

    ON_DEBUG(DEBUG_INFO, "Program validation");

    //* Verify directory argument */
    if (args_info.dir_given)
    {
        if (stat(args_info.dir_arg, &f_stat) == 0)
        {
            if (!S_ISDIR(f_stat.st_mode))
            {
                ON_DEBUG(DEBUG_ERROR, "'%s' is not a directory.", args_info.dir_arg);
                ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a directory.", args_info.dir_arg);
            }
        }
        else
        {
            ON_DEBUG(DEBUG_ERROR, "Directory '%s' not found.", args_info.dir_arg);
            ERROR(ARGUMENT_INCORRECT, "Unable to get directory properties. Make sure '%s' exists.", args_info.dir_arg);
        }
    }

    //* Verify if file or batch file is given */
    if (!(args_info.file_given || args_info.batch_given))
    {
        ON_DEBUG(DEBUG_ERROR, "Missing argument for execution ('batch' or 'file').");
        ERROR(ARGUMENT_MISSING, "Insuficient arguments. (required 'batch' or 'file')");
    }
    ON_DEBUG(DEBUG_INFO, "A required argument was given");

    //* Verify single file input */
    if (args_info.file_given)
    {
        char *filepath = args_info.file_arg;

        if (args_info.dir_given)
        {
            strcpy(filepath, args_info.dir_arg);
            strcat(filepath, args_info.batch_arg);
            ON_DEBUG(DEBUG_INFO, "Working directory given. ('%s')", filepath);
        }

        if (stat(filepath, &f_stat) == 0)
        {
            if (!S_ISREG(f_stat.st_mode))
            {
                ON_DEBUG(DEBUG_ERROR, "'%s' is not a regular file.", filepath);
                ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a file.", filepath);
            }
            strcpy(queue_files[queue_counter], filepath);
            queue_counter++;
            ON_DEBUG(DEBUG_INFO, "Input file is valid and is added to the queue");
        }
        else
        {
            ON_DEBUG(DEBUG_ERROR, "'%s' not found.", filepath);
            ERROR(ARGUMENT_INCORRECT, "Unable to get file properties. Make sure '%s' exists.", filepath);
        }
    }

    // //* Verify batch file input */
    if (args_info.batch_given)
    {
        if (stat(args_info.batch_arg, &f_stat) == 0)
        {
            if (!S_ISREG(f_stat.st_mode))
            {
                ON_DEBUG(DEBUG_ERROR, "'%s' is not a regular file.", args_info.batch_arg);
                ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a file.", args_info.batch_arg);
            }
            ON_DEBUG(DEBUG_INFO, "Batch input file is valid. Processing contents...");

            int fd = open(args_info.batch_arg, O_RDONLY);
            int fs = file_size(fd);
            char c;
            int readed = 0;

            int linebuffer_counter = 0;
            char linebuffer[fs + 2];
            linebuffer[fs] = '\0';

            while ((readed = read(fd, &c, 1)) > 0)
            {
                if (c == '\r')
                {
                    continue;
                }

                if (c == '\n' || c == '\0')
                {
                    linebuffer[linebuffer_counter] = '\0';
                    linebuffer_counter = 0;

                    if (strlen(linebuffer) > 0)
                    {
                        if (strlen(linebuffer) < MAX_FILE_PATH)
                        {
                            if (stat(linebuffer, &f_stat) == 0)
                            {
                                if (!S_ISREG(f_stat.st_mode))
                                {
                                    ON_DEBUG(DEBUG_ERROR, "'%s' is not a regular file.", linebuffer);
                                    // MESSAGE(MESSAGE_WARN, "Skipping file '%s'. Make sure its a file.", linebuffer);
                                }
                                else
                                {
                                    strcpy(queue_files[queue_counter], &linebuffer[0]);
                                    queue_counter++;
                                    ON_DEBUG(DEBUG_INFO, "Found line with ( %s )", linebuffer);
                                }
                            }
                            else
                            {
                                ON_DEBUG(DEBUG_ERROR, "'%s' not found.", linebuffer);
                                // MESSAGE(MESSAGE_WARN, "Unable to get file properties. Make sure '%s' exists.", linebuffer);
                            }
                        }
                        else
                        {
                            ON_DEBUG(DEBUG_WARN, "Path limit (%d) exceded skipping '%s'", MAX_FILE_PATH, linebuffer);
                        }
                    }
                }
                else
                {
                    linebuffer[linebuffer_counter] = c;
                    linebuffer_counter++;
                }
            }

            //* File ended
            if (readed == 0)
            {
                linebuffer[linebuffer_counter] = '\0';
                linebuffer_counter = 0;

                if (strlen(linebuffer) > 0)
                {
                    if (strlen(linebuffer) < MAX_FILE_PATH)
                    {
                        if (stat(linebuffer, &f_stat) == 0)
                        {
                            if (!S_ISREG(f_stat.st_mode))
                            {
                                ON_DEBUG(DEBUG_WARN, "'%s' is not a regular file.", linebuffer);
                                // MESSAGE(MESSAGE_WARN, "Skipping file '%s'. Make sure its a file.", linebuffer);
                            }
                            else
                            {
                                strcpy(queue_files[queue_counter], linebuffer);
                                queue_counter++;
                                ON_DEBUG(DEBUG_INFO, "Found line with ( %s )", linebuffer);
                            }
                        }
                        else
                        {
                            ON_DEBUG(DEBUG_ERROR, "'%s' not found.", linebuffer);
                            // MESSAGE(MESSAGE_WARN, "Unable to get file properties. Make sure '%s' exists.", linebuffer);
                        }
                    }
                    else
                    {
                        ON_DEBUG(DEBUG_WARN, "Path limit (%d) exceded skipping '%s'", MAX_FILE_PATH, linebuffer);
                    }
                }
            }

            close(fd);
        }
        else
        {
            ON_DEBUG(DEBUG_ERROR, "'%s' not found.", args_info.file_arg);
            ERROR(ARGUMENT_INCORRECT, "Unable to get file properties. Make sure '%s' exists.", args_info.file_arg);
        }
    }

    cmdline_parser_free(&args_info);

    if (queue_counter == 0)
    {
        ON_DEBUG(DEBUG_ERROR, "No valid paths were found within the given arguments.");
        ERROR(EMPTY_FILE, "No valid paths were found.");
    }

    ON_DEBUG(DEBUG_INFO, "Everything ok. Ready to start processing");

    // TODO processing
    // queue_files array
    // queue_counter

    exit(EXIT_SUCCESS);
    return 0;
}
