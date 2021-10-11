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
#include <sys/wait.h>
#include <sys/sysmacros.h>

#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <dirent.h>
// #include <time.h>
// #include <assert.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "message.h"
#include "file_helper.h"
#include "string_aux.h"
#include "checkfile.h"

#define UNABLE_OPEN_FILE 2
#define UNABLE_CLOSE_FILE 3
#define LIMIT_EXCEEDED 4
#define UNABLE_OPEN_DIR 5
#define UNABLE_CLOSE_DIR 6
#define UNABLE_START_PROCESS 7
#define EXECUTION_FAILURE 8

#define MAX_QUEUE 100
#define MAX_STRING_SIZE 256
#define TMP_FILE "tmp"

struct gengetopt_args_info args;

int supported_extensions_count = 7;
const char *supported_extensions[] = {"pdf", "gif", "jpg", "png", "mp4", "zip", "html"};

char files_queue[MAX_QUEUE][MAX_STRING_SIZE];
int queue_counter = 0;

int main(int argc, char *argv[])
{
    //* Disable warnings */
    (void)argc;
    (void)argv;

    if (cmdline_parser(argc, argv, &args) != 0)
    {
        exit(0);
    }

    // Clean queue
    for (int i = 0; i < MAX_QUEUE; i++)
    {
        for (int j = 0; j < MAX_STRING_SIZE; j++)
        {
            files_queue[i][j] = '\0';
        }
    }

    MESSAGE(MESSAGE_PROCESSING, "Processing...");
    /*
        Processing Magic
        To test Signals
    */
    sleep(2);

    // File argument
    if (args.file_given)
    {
        int i = 0;
        while (args.file_arg[i] != NULL)
        {
            if (file_exists(args.file_arg[i]) && is_regular_file(args.file_arg[i]))
            {
                if (strlen(args.file_arg[i]) >= MAX_STRING_SIZE)
                {
                    ON_DEBUG(UNABLE_OPEN_FILE, "max string size exceded in '%s'", args.file_arg[i]);
                    MESSAGE(MESSAGE_ERROR, "cannot open file '%s' - %s", strerror(ENAMETOOLONG));
                }
                else
                {
                    strcpy(files_queue[queue_counter], args.file_arg[i]);
                    queue_counter++;
                    ON_DEBUG(MESSAGE_INFO, "Added new file to files_queue ( %s )", args.file_arg[i]);
                }
            }
            else
            {
                ON_DEBUG(UNABLE_OPEN_FILE, "cannot open file '%s'", args.file_arg[i]);
                MESSAGE(MESSAGE_ERROR, "cannot open file '%s' - %s", strerror(ENOENT));
            }
            i++;
        }
    }

    if (args.batch_given)
    {
        if (file_exists(args.batch_arg) && is_regular_file(args.batch_arg))
        {
            int fd = open_file(args.batch_arg, O_RDONLY), fs = file_size(fd), readed = 0, linebuffer_counter = 0;
            char linebuffer[fs + 2], c;
            linebuffer[fs] = '\0';

            while ((readed = read(fd, &c, 1)) >= 0)
            {
                if (c == '\r')
                {
                    continue;
                }

                //* End a line
                if (c == '\n' || c == '\0' || readed == 0)
                {
                    // Make sure the line ends
                    linebuffer[linebuffer_counter] = '\0';
                    linebuffer_counter = 0;

                    // Confirm that the line has text
                    if (strlen(linebuffer) > 0)
                    {
                        // And does not exceed the MAX_STRING_SIZE
                        if (strlen(linebuffer) < MAX_STRING_SIZE)
                        {
                            if (file_exists(linebuffer) && is_regular_file(linebuffer))
                            {
                                strcpy(files_queue[queue_counter], linebuffer);
                                queue_counter++;
                                ON_DEBUG(MESSAGE_INFO, "Added new file to files_queue ( %s )", linebuffer);
                            }
                            else
                            {
                                MSG_ENOENT(linebuffer);
                            }
                        }
                        else
                        {
                            MSG_NAMETOOLONG(linebuffer);
                            continue;
                        }
                    }
                }
                else
                {
                    // Adds to the buffer while no line ends
                    linebuffer[linebuffer_counter] = c;
                    linebuffer_counter++;
                }

                if (readed == 0)
                {
                    break;
                }
            }
        }
        else
        {
            char msg[MAX_STRING_SIZE];
            strcpy(msg, args.batch_arg);
            cmdline_parser_free(&args);
            ERROR(UNABLE_OPEN_FILE, "cannot open file '%s'", msg);
        }
    }

    if (args.dir_given)
    {
        int len_dir = strlen(args.dir_arg);
        char string_dir[len_dir + 2];
        strcpy(string_dir, args.dir_arg);

        if (string_dir[len_dir - 1] != '/')
        {
            string_dir[len_dir] = '/';
            string_dir[len_dir + 1] = '\0';
        }

        DIR *directory;
        struct dirent *entity;
        directory = opendir(string_dir);
        if (directory == NULL)
        {
            cmdline_parser_free(&args);
            ERROR(UNABLE_OPEN_DIR, "cannot open dir '%s'", string_dir);
        }
        while ((entity = readdir(directory)) != NULL)
        {
            // if (entity->d_type == DT_REG) // Sometimes it doesnt work...
            // https://stackoverflow.com/questions/5114396/dt-reg-undeclared-first-use-in-this-function-and-std-c99
            if (entity->d_type == 8)
            {
                char full[MAX_STRING_SIZE];
                sprintf(full, "%s%s", string_dir, entity->d_name);

                if (strlen(full) >= MAX_STRING_SIZE)
                {
                    MSG_NAMETOOLONG(full);
                }
                else
                {
                    strcpy(files_queue[queue_counter], full);
                    queue_counter++;
                    ON_DEBUG(MESSAGE_INFO, "Added new file to files_queue ( %s )", full);
                }
            }
        }
        closedir(directory);
    }

    cmdline_parser_free(&args);

    pid_t pid = fork();
    if (pid == -1)
    {
        ERROR(UNABLE_START_PROCESS, "Unable to start new process on %d", getpid());
    }

    if (pid == 0)
    {
        char *exec_arguments[MAX_STRING_SIZE];
        exec_arguments[0] = "file";
        exec_arguments[1] = "--mime-type";
        int tmp_counter = 2;

        for (int i = 0; i < queue_counter; i++)
        {
            exec_arguments[i + tmp_counter] = files_queue[i];
        }
        tmp_counter += queue_counter;
        exec_arguments[tmp_counter] = NULL;

        int fd = open(TMP_FILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (fd == -1)
        {
            ERROR(UNABLE_OPEN_FILE, "[%d] Unable to create/open temporary file.", getpid());
        }

        // Copy of stdout to file
        dup2(fd, STDOUT_FILENO);
        close(fd);
        execvp("file", exec_arguments);

        ERROR(EXECUTION_FAILURE, "Process was unable to execute commands.");
    }

    waitpid(pid, NULL, 0);

    // * Processing files_queue
    if (file_exists(TMP_FILE) && is_regular_file(TMP_FILE))
    {
        int fd = open_file(TMP_FILE, O_RDONLY), fs = file_size(fd), readed = 0, linebuffer_counter = 0;
        char linebuffer[fs + 2], c;
        linebuffer[fs] = '\0';

        while ((readed = read(fd, &c, 1)) >= 0)
        {
            if (c == '\r')
            {
                continue;
            }

            //* End a line
            if (c == '\n' || c == '\0' || readed == 0)
            {
                // Make sure the line ends
                linebuffer[linebuffer_counter] = '\0';
                linebuffer_counter = 0;

                // Confirm that the line has text
                if (strlen(linebuffer) > 0)
                {
                    // And does not exceed the MAX_STRING_SIZE
                    char file[MAX_QUEUE];
                    char ext[MAX_QUEUE];
                    char type[MAX_QUEUE];
                    strcut(file,
                           linebuffer,
                           0,
                           strlen(linebuffer) - strlen(strchr(linebuffer, ' ')) - 2);
                    strcut(ext,
                           linebuffer,
                           strlen(linebuffer) - strlen(strchr(linebuffer, '.')) + 1,
                           strlen(linebuffer) - strlen(strchr(linebuffer, ':')) - 1);
                    strcut(type,
                           linebuffer,
                           strlen(linebuffer) - strlen(strrchr(linebuffer, '/')) + 1,
                           strlen(linebuffer));

                    // Just for the extra output for the file
                    if (strlen(linebuffer) < (MAX_STRING_SIZE + 20))
                    {
                        if (check(type, ext))
                        {
                            MESSAGE(MESSAGE_OK, "'%s': extension '%s' matches file type '%s'", file, ext, type);
                        }
                        else
                        {
                            MESSAGE(MESSAGE_MISMATCH, "'%s': extension '%s', file type '%s'", file, ext, type);
                        }
                    }
                    else
                    {
                        MESSAGE(MESSAGE_ERROR, "max string size exceded in '%s' - %s", file, strerror(ENAMETOOLONG));
                    }
                }
            }
            else
            {
                // Adds to the buffer while no line ends
                linebuffer[linebuffer_counter] = c;
                linebuffer_counter++;
            }

            if (readed == 0)
            {
                break;
            }
        }
    }
    else
    {
        ERROR(UNABLE_OPEN_FILE, "cannot open file '%s' - %s", TMP_FILE, strerror(ENOENT));
    }

    printf("\n");

    return EXIT_SUCCESS;
}
