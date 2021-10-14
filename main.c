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
#include <time.h>
// #include <assert.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "message.h"
#include "file_helper.h"
#include "string_aux.h"
#include "checkfile.h"

#define MAX_QUEUE 100
#define MAX_STRING_SIZE 256
#define TMP_FILE "tmp"

void add_to_queue(char *string);
void handle_signal(int signal, siginfo_t *info, void *context);

time_t timestamp;
struct tm *timeinfo;
struct gengetopt_args_info args;

int supported_extensions_count = 7;
const char *supported_extensions[] = {"pdf", "gif", "jpg", "png", "mp4", "zip", "html"};

char *current_file_in_batch = NULL;
char files_queue[MAX_QUEUE][MAX_STRING_SIZE];
int queue_counter = 0;

int counter_analized = 0;
int counter_ok = 0;
int counter_mismatch = 0;
int counter_error = 0;

int display_summary = 0;
int batch_processing = 0;

/*
 * ──────────────────────────────────────────────────────────────── MAIN CODE ─────
 */

int main(int argc, char *argv[])
{
    //* Disable warnings */
    (void)argc;
    (void)argv;

    // Validate input
    if (cmdline_parser(argc, argv, &args) != 0)
    {
        exit(0);
    }

    // Setting up this settings as soon as possible because they are used in signals
    if (args.batch_given)
    {
        batch_processing = 1;
        time(&timestamp);
        timeinfo = localtime(&timestamp);
    }

    // Signal assign
    struct sigaction act;
    act.sa_sigaction = handle_signal;
    sigemptyset(&act.sa_mask);              // Remove signal blocking
    act.sa_flags = SA_SIGINFO | SA_RESTART; // Sends adicional info to the handler and recover blocking calls

    if (sigaction(SIGQUIT, &act, NULL) < 0)
    {
        ERROR_UNABLE_SET_SIGNAL_HANDLER(SIGQUIT);
    }

    if (sigaction(SIGUSR1, &act, NULL) < 0)
    {
        ERROR_UNABLE_SET_SIGNAL_HANDLER(SIGUSR1);
    }

    // Clean queue
    for (int i = 0; i < MAX_QUEUE; i++)
    {
        for (int j = 0; j < MAX_STRING_SIZE; j++)
        {
            files_queue[i][j] = '\0';
        }
    }

    MESSAGE(MESSAGE_PROCESSING, "Starting...");

    /*
     * ──────────────────────────────────────────────────────────── ARGUMENT INIT ─────
     */

    // File argument
    if (args.file_given)
    {
        MESSAGE(MESSAGE_INFO, "Analizing...");

        int i = 0;
        while (args.file_arg[i] != NULL)
        {
            if (!file_exists(args.file_arg[i]))
            {
                ERROR_FILE_NOT_EXISTS(args.file_arg[i]);
            }

            if (!is_regular_file(args.file_arg[i]))
            {
                ERROR_INCORRECT_FILE_ARG(args.file_arg[i]);
            }

            if (strlen(args.file_arg[i]) > MAX_STRING_SIZE)
            {
                MSG_FILE_TOOLONG(args.file_arg[i]);
            }
            else
            {
                add_to_queue(args.file_arg[i]);
            }

            i++;
        }
    }

    // Batch argument
    if (args.batch_given)
    {
        display_summary = 1;

        if (!args.dir_given && !args.file_given)
        {
            MESSAGE(MESSAGE_INFO, "Analizing files listed in '%s'...", args.batch_arg);
        }

        if (strcmp(file_extension(args.batch_arg), "txt") != 0)
        {
            counter_error++;
            char msg[MAX_STRING_SIZE];
            strcpy(msg, args.batch_arg);
            cmdline_parser_free(&args);
            ERROR_INCORRECT_FILE_ARG(msg);
        }

        if (!file_exists(args.batch_arg))
        {
            counter_error++;
            char msg[MAX_STRING_SIZE];
            strcpy(msg, args.batch_arg);
            cmdline_parser_free(&args);
            ERROR_FILE_NOT_EXISTS(msg);
        }

        if (!is_regular_file(args.batch_arg))
        {
            counter_error++;
            char msg[MAX_STRING_SIZE];
            strcpy(msg, args.batch_arg);
            cmdline_parser_free(&args);
            ERROR_INCORRECT_FILE_ARG(msg);
        }

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
                    counter_analized++;
                    // Checks if the record exceeds the MAX_STRING_SIZE
                    if (strlen(linebuffer) > MAX_STRING_SIZE)
                    {
                        counter_error++;
                        MSG_FILE_TOOLONG(linebuffer);
                        continue;
                    }

                    if (!file_exists(linebuffer))
                    {
                        counter_error++;
                        MSG_FILE_NOT_EXISTS(linebuffer);
                        continue;
                    }

                    if (!is_regular_file(linebuffer))
                    {
                        counter_error++;
                        MSG_INCORRECT_FILE_ARG(linebuffer);
                        continue;
                    }

                    add_to_queue(linebuffer);
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

    // Dir argument
    if (args.dir_given)
    {
        display_summary = 1;
        if (!args.file_given && !args.batch_given)
        {
            MESSAGE(MESSAGE_INFO, "Analizing files listed in '%s' directory...", args.dir_arg);
        }

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
            ERROR_CANT_OPEN_DIR(string_dir);
        }
        while ((entity = readdir(directory)) != NULL)
        {
            // if (entity->d_type == DT_REG) // Sometimes it doesnt work...
            // https://stackoverflow.com/questions/5114396/dt-reg-undeclared-first-use-in-this-function-and-std-c99
            if (entity->d_type == 8)
            {
                counter_analized++;
                char full[MAX_STRING_SIZE];
                sprintf(full, "%s%s", string_dir, entity->d_name);

                if (strlen(full) > MAX_STRING_SIZE)
                {
                    counter_error++;
                    cmdline_parser_free(&args);
                    MSG_FILE_TOOLONG(full);
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

    // Free unused memory
    cmdline_parser_free(&args);

    /*
     * ───────────────────────────────────────────────────────────── FORK PROCESS ─────
     */

    pid_t pid = fork();
    if (pid == -1)
    {
        ERROR_CANT_START_PROC();
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
            ERROR_CANT_OPEN_FILE(getpid());
        }

        // Copy of stdout to file
        dup2(fd, STDOUT_FILENO);
        close(fd);
        execvp("file", exec_arguments);

        ERROR_CANT_EXECUTE_PROC();
    }

    waitpid(pid, NULL, 0);

    /*
     * ─────────────────────────────────────────────────────── FORK OUPUT PROCESS ─────
     */

    if (!file_exists(TMP_FILE))
    {
        ERROR_FILE_NOT_EXISTS(TMP_FILE);
    }

    if (!is_regular_file(TMP_FILE))
    {
        ERROR_INCORRECT_FILE_ARG(TMP_FILE);
    }

    FILE *fl = fopen(TMP_FILE, "r");
    if (fl == NULL)
    {
        ERROR_CANT_OPEN_FILE(TMP_FILE);
    }

    ssize_t readed = 0;
    size_t lenght = 0;
    char *linebuffer = NULL;

    while ((readed = getline(&linebuffer, &lenght, fl)) != EOF)
    {
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

            current_file_in_batch = file;

            strcut(ext,
                   linebuffer,
                   strlen(linebuffer) - strlen(strchr(linebuffer, '.')) + 1,
                   strlen(linebuffer) - strlen(strchr(linebuffer, ':')) - 1);
            strcut(type,
                   linebuffer,
                   strlen(linebuffer) - strlen(strrchr(linebuffer, '/')) + 1,
                   strlen(linebuffer) - 2);

            if (!array_has_string(supported_extensions, supported_extensions_count, ext))
            {
                counter_error++;
                MESSAGE(MESSAGE_ERROR, "'%s': type '%s' extension not supported", file, ext);
                continue;
            }

            // Just for the extra output for the file
            if (strlen(linebuffer) < (MAX_STRING_SIZE))
            {
                if (check(type, ext))
                {
                    counter_ok++;
                    MESSAGE(MESSAGE_OK, "'%s': extension '%s' matches file type '%s'", file, ext, type);
                }
                else
                {
                    counter_mismatch++;
                    MESSAGE(MESSAGE_MISMATCH, "'%s': extension '%s', file type '%s'", file, ext, type);
                }
            }
            else
            {
                counter_error++;
                MESSAGE(MESSAGE_ERROR, "max string size exceded in '%s' - %s", file, strerror(ENAMETOOLONG));
            }
        }
    }

    // Cleanup memory
    unlink(TMP_FILE);
    fclose(fl);
    free(linebuffer);

    // Summary display if
    if (display_summary)
    {
        printf("\n");
        MESSAGE(
            MESSAGE_SUMMARY,
            "\n\tFiles analyzed: %d\n\tfiles OK: %d\n\tFiles MISMATCH: %d\n\tErrors: %d",
            counter_analized, counter_ok, counter_mismatch, counter_error);
    }

    printf("\n");
    return EXIT_SUCCESS;
}

/*
 * ───────────────────────────────────────────────────────── HELPER FUNCTIONS ─────
 */

/**
 * @brief Add to queue helper
 *
 * @param string
 */
void add_to_queue(char *string)
{
    strcpy(files_queue[queue_counter], string);
    queue_counter++;
    ON_DEBUG(MESSAGE_INFO, "Added new file to files_queue ( %s )", string);
}

/**
 * @brief Signals handler
 *
 * @param signal
 * @param info
 * @param context
 */
void handle_signal(int signal, siginfo_t *info, void *context)
{
    (void)context;

    int aux = errno;

    switch (signal)
    {
    case SIGQUIT:
        printf("\n");
        MESSAGE(MESSAGE_SIGNAL, "\nCaptured %s signal (sent by PID: %ld). Use %s to terminate application.\n", strsignal(SIGQUIT), info->si_pid, strsignal(SIGINT));
        break;
    case SIGUSR1:;
        if (batch_processing)
        {
            printf("\n");
            // fix for "labels": https://www.educative.io/edpresso/resolving-the-a-label-can-only-be-part-of-a-statement-error
            char formatted_time[MAX_STRING_SIZE];
            strftime(formatted_time, sizeof(formatted_time), "%Y.%m.%d_%Hh%M:%S", timeinfo);
            MESSAGE(MESSAGE_SIGNAL, "\n\t%s\n\tNº%d/%s\n", formatted_time, (counter_analized + 1), (current_file_in_batch == NULL) ? "'Not enough time to get the file'" : current_file_in_batch);
        }
        break;
    default:
        break;
    }

    errno = aux;
}