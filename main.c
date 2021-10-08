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
#include "file_helper.h"

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
#define UNABLE_TO_CLOSE 13

struct gengetopt_args_info args_info;

int main(int argc, char *argv[])
{
    //* Disable warnings */
    (void)argc;
    (void)argv;

    MESSAGE(MESSAGE_INFO, "Checkfile starting...");

    ON_DEBUG(DEBUG_INFO, "Gengetopt validation.");
    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(0);
    }

    char queue_files[MAX_QUEUE][MAX_FILE_PATH];
    int queue_counter = 0;

    ON_DEBUG(DEBUG_INFO, "Program validation.");

    //* Verify directory argument */
    if (args_info.dir_given)
    {
        if (!file_exists(args_info.dir_arg))
        {
            ERROR(ARGUMENT_INCORRECT, "Directory does not exist. (%s)", args_info.dir_arg);
        }

        if (!is_directory(args_info.dir_arg))
        {
            ERROR(ARGUMENT_INCORRECT, "Make sure '%s' exists.", args_info.dir_arg);
        }

        chdir(args_info.dir_arg);
        MESSAGE(MESSAGE_INFO, "Working in the given directory. ('%s')", args_info.dir_arg);
    }

    //* Verify if file or batch file is given */
    if (!(args_info.file_given || args_info.batch_given))
    {
        ERROR(ARGUMENT_MISSING, "Insuficient arguments. (required 'batch' or 'file')");
    }

    ON_DEBUG(DEBUG_INFO, "A required argument was given.");
    MESSAGE(MESSAGE_INFO, "Validating input...");

    //* Verify single file input */
    if (args_info.file_given)
    {
        if (!file_exists(args_info.file_arg))
        {
            ERROR(ARGUMENT_INCORRECT, "Make sure '%s' exists.", args_info.file_arg);
        }

        if (!is_regular_file(args_info.file_arg))
        {
            ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a regular file.", args_info.file_arg);
        }

        strcpy(queue_files[queue_counter], args_info.file_arg);
        queue_counter++;
        ON_DEBUG(DEBUG_INFO, "Input file is valid and is added to the queue");
    }

    //* Verify batch file input */
    if (args_info.batch_given)
    {
        if (!file_exists(args_info.batch_arg))
        {
            ERROR(ARGUMENT_INCORRECT, "Make sure '%s' exists.", args_info.file_arg);
        }

        if (!is_regular_file(args_info.batch_arg))
        {
            ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a file and exists.", args_info.batch_arg);
        }

        ON_DEBUG(DEBUG_INFO, "Batch input file is valid. Processing contents...");

        int fd = open_file(args_info.batch_arg, O_RDONLY);
        int fs = file_size(fd);
        char c;
        int readed = 0;

        int linebuffer_counter = 0;
        char linebuffer[fs + 2];
        linebuffer[fs] = '\0';

        ON_DEBUG(DEBUG_INFO, "Starting to read batch file contents...");
        while ((readed = read(fd, &c, 1)) > 0)
        {
            if (c == '\r')
            {
                continue;
            }

            //* End a line
            if (c == '\n' || c == '\0')
            {
                // Make sure the line ends
                linebuffer[linebuffer_counter] = '\0';
                linebuffer_counter = 0;

                // Confirm that the line has text
                if (strlen(linebuffer) > 0)
                {
                    // And does not exceed the MAX_FILE_PATH
                    if (strlen(linebuffer) < MAX_FILE_PATH)
                    {
                        if (!file_exists(linebuffer))
                        {
                            MESSAGE(MESSAGE_WARN, "'%s' does not exist. Skipping...", linebuffer);
                            continue;
                        }

                        if (!is_regular_file(linebuffer))
                        {
                            MESSAGE(MESSAGE_WARN, "'%s' is not a regular file. Skipping...", linebuffer);
                            continue;
                        }

                        strcpy(queue_files[queue_counter], &linebuffer[0]);
                        queue_counter++;
                        ON_DEBUG(DEBUG_INFO, "Found line with ( %s )", linebuffer);
                    }
                    else
                    {
                        MESSAGE(MESSAGE_WARN, "Path limit (%d) exceded skipping '%s'", MAX_FILE_PATH, linebuffer);
                    }
                }
            }
            else
            {
                // Adds to the buffer while no line ends
                linebuffer[linebuffer_counter] = c;
                linebuffer_counter++;
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
                        if (!file_exists(linebuffer))
                        {
                            MESSAGE(MESSAGE_WARN, "'%s' does not exist. Skipping...", linebuffer);
                            continue;
                        }

                        if (!is_regular_file(linebuffer))
                        {
                            ON_DEBUG(MESSAGE_WARN, "'%s' is not a regular file.", linebuffer);
                            MESSAGE(MESSAGE_WARN, "'%s' is not a regular file. Skipping...", linebuffer);
                        }

                        strcpy(queue_files[queue_counter], &linebuffer[0]);
                        queue_counter++;
                        ON_DEBUG(DEBUG_INFO, "Found line with ( %s )", linebuffer);
                    }
                    else
                    {
                        ON_DEBUG(DEBUG_WARN, "Path limit (%d) exceded skipping '%s'", MAX_FILE_PATH, linebuffer);
                    }
                }
            }
        }

        int is_closed = close(fd);

        if (is_closed == -1)
        {
            ERROR(UNABLE_TO_CLOSE, "Unable to close batch file.");
        }

        ON_DEBUG(DEBUG_INFO, "Closed batch file.");
    }

    ON_DEBUG(DEBUG_INFO, "Freed up gengetopt struct.");
    cmdline_parser_free(&args_info);

    if (queue_counter == 0)
    {
        ERROR(EMPTY_FILE, "No valid paths were found in the input.");
    }

    MESSAGE(MESSAGE_OK, "Found %d files to check.", queue_counter);

    ON_DEBUG(DEBUG_INFO, "Everything ok. Ready to start processing");

    // TODO processing
    // queue_files array
    // queue_counter

    exit(EXIT_SUCCESS);
    return 0;
}
