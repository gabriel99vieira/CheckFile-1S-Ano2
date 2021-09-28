/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <time.h>
// #include <fcntl.h>
// #include <stdint.h>
// #include <ctype.h>
// #include <signal.h>
// #include <sys/sysmacros.h>
// #include <sys/stat.h>
// #include <assert.h>
// #include <dirent.h>

// #include "debug.h"
// #include "memory.h"
// #include "args.h"

// struct gengetopt_args_info args_info;

int main(int argc, char *argv[])
{
    /* Disable warnings */
    (void)argc;
    (void)argv;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
    	exit(0);
    }

    // Save what is needed to avoid memory consumption

    cmdline_parser_free(&args_info);

    /* Main code */
    DEBUG("Main is empty -- add code here");


    return 0;
}
