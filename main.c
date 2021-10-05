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
// #include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
// #include <sys/wait.h>
// #include <sys/sysmacros.h>

#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
// #include <dirent.h>
// #include <time.h>
// #include <stdint.h>
// #include <assert.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "debug_aux.h"

#define ARGUMENT_MISSING 10
#define ARGUMENT_INCORRECT 11

struct gengetopt_args_info args_info;

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

    char *queue_files[100];
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
                ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a directory.", args_info.dir_arg);
            }
        }
        else
        {
            ERROR(ARGUMENT_INCORRECT, "Unable to get directory properties. Make sure '%s' exists.", args_info.dir_arg);
        }
    }

    //* Verify if file or batch file is given */
    if (!(args_info.file_given || args_info.batch_given))
    {
        ERROR(ARGUMENT_MISSING, "Insuficient arguments. (required 'batch' or 'file')");
    }
    ON_DEBUG(DEBUG_INFO, "A required argument was given");

    //* Verify single file input */
    if (args_info.file_given)
    {
        if (stat(args_info.file_arg, &f_stat) == 0)
        {
            if (!S_ISREG(f_stat.st_mode))
            {
                ERROR(ARGUMENT_INCORRECT, "Make sure '%s' is a file.", args_info.file_arg);
            }
            queue_files[queue_counter] = args_info.file_arg;
            queue_counter++;
            ON_DEBUG(DEBUG_INFO, "Input file is valid and is added to the queue");
        }
    }
    else
    {
        ERROR(ARGUMENT_INCORRECT, "Unable to get file properties. Make sure '%s' exists.", args_info.file_arg);
    }

    //* Verify batch file input */
    // TODO

    cmdline_parser_free(&args_info);

    ON_DEBUG(DEBUG_INFO, "Everything ok. Ready to start processing");

    return 0;
}
