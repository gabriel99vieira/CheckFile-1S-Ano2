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

#define MAX_QUEUE 100
#define MAX_STRING_SIZE 256
#define TMP_FILE "tmp"

#define TMP_BATCH TMP_FILE "_batch"
#define TMP_SOUT TMP_FILE "_out"
#define TMP_SERR TMP_FILE "_err"

/**
 * @brief Checks file extension with its mime type
 *
 * @param pos
 * @param readed_type
 * @return int
 */
int check(int pos, const char *readed_type);

/**
 * @brief Add to queue helper
 *
 * @param string
 */
void add_to_queue(char *string);

/**
 * @brief Signals handler
 *
 * @param signal
 * @param info
 * @param context
 */
void handle_signal(int signal, siginfo_t *info, void *context);

void argument_file();
void argument_batch();
void argument_directory();

#pragma region INIT_VAR

time_t timestamp;
struct tm *timeinfo;
struct gengetopt_args_info args;

#define SUPPORT_COUNT 7
int supported_extensions_count = SUPPORT_COUNT;
const char *supported_extensions[] = {"pdf", "gif", "jpg", "png", "mp4", "zip", "html"};

int supported_types_count = SUPPORT_COUNT;
const char *supported_types[] = {"application/pdf", "image/gif", "image/jpeg", "image/png", "video/mp4", "application/zip", "text/html"};

// Queue
char *current_file_in_batch = NULL;
char files_queue[MAX_QUEUE][MAX_STRING_SIZE];
int queue_counter = 0;

// Counters
int counter_analized = 0;
int counter_ok = 0;
int counter_mismatch = 0;
int counter_error = 0;

// Booleans
int display_summary = 0;
int batch_processing = 0;

#pragma endregion INIT_VAR

/*
 * ──────────────────────────────────────────────────────────────── MAIN CODE ─────
 */

int main(int argc, char *argv[])
{
    //* Disable warnings */
    (void)argc;
    (void)argv;

    MESSAGE(MESSAGE_INFO, "Starting ...");

    // Validate input
    if (cmdline_parser(argc, argv, &args) != 0)
    {
        exit(0);
    }

    // Double checking paramenters
    if (!args.file_given && !args.batch_given && !args.dir_given)
    {
        ON_ERROR(C_ERROR_INCRRECT_OR_INVALID_ARG, "at least an argument must be given");
    }

    // Setting up this settings as soon as possible because they are used in signals
    if (args.batch_given)
    {
        batch_processing = 1;
        time(&timestamp);
        timeinfo = localtime(&timestamp);
    }

    /*
     * ────────────────────────────────────────────────────────────────── SIGNALS ─────
     */
#pragma region SIGNALS
    struct sigaction act;
    act.sa_sigaction = handle_signal;       // handler function
    sigemptyset(&act.sa_mask);              // Remove signal blocking
    act.sa_flags = SA_SIGINFO | SA_RESTART; // Sends adicional info to the handler and recover blocking calls

    if (sigaction(SIGQUIT, &act, NULL) < 0)
    {
        ERROR_UNABLE_SET_SIGNAL_HANDLER(SIGQUIT);
    }

    if (batch_processing)
    {
        if (sigaction(SIGUSR1, &act, NULL) < 0)
        {
            ERROR_UNABLE_SET_SIGNAL_HANDLER(SIGUSR1);
        }
    }
    else
    {
        // sigaddset(&act.sa_mask, SIGUSR1);
        signal(SIGQUIT, SIG_IGN);
    }

#pragma endregion SIGNALS

    // Clean queue
    for (int i = 0; i < MAX_QUEUE; i++)
    {
        for (int j = 0; j < MAX_STRING_SIZE; j++)
        {
            files_queue[i][j] = '\0';
        }
    }

    /*
     * ──────────────────────────────────────────────────────────── ARGUMENT INIT ─────
     */

#pragma region ARGUMENT

    // File argument
    argument_file();

    // Batch argument
    argument_batch();

    // Dir argument
    argument_directory();

    /**
     * ! Add here your new way to select files
     * ! Just add them to the buffer like this
     * ! add_to_queue(file_string)
     */

    // Free unused memory
    cmdline_parser_free(&args);

    // Prints messagens in buffer
    fflush(stdout);

#pragma endregion ARGUMENT

    /*
     * ───────────────────────────────────────────────────────────── FORK PROCESS ─────
     */

    /*
        ! A buffer will not be used to retrieve the child process
        ! Instead a temporary file is created to allow more output
    */

#pragma region CHILD_PROCESS

    // Create fork
    pid_t pid = fork();
    if (pid == -1)
    {
        ERROR_CANT_START_PROC();
    }

    // Child process distinction
    if (pid == 0)
    {
        int fdbatch = open(TMP_BATCH, O_CREAT | O_RDWR | O_TRUNC, 0644);

        for (int i = 0; i < queue_counter; i++)
        {
            char buf[MAX_STRING_SIZE + 1];
            strcpy(buf, files_queue[i]);
            buf[strlen(files_queue[i])] = '\n';
            buf[strlen(files_queue[i]) + 1] = '\0';
            ssize_t written = write(fdbatch, buf, strlen(buf));
            if (written < 0)
            {
                close(fdbatch);
                ERROR_CANT_WRITE_FILE(TMP_BATCH);
            }
        }

        close(fdbatch);

        int fdout = open(TMP_SOUT, O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (fdout == -1)
        {
            ERROR_CANT_OPEN_FILE(TMP_SOUT);
        }

        int fderr = open(TMP_SERR, O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (fderr == -1)
        {
            close(fdout);
            ERROR_CANT_OPEN_FILE(TMP_SERR);
        }

        // Copy of io buffers
        dup2(fdout, STDOUT_FILENO);
        dup2(fderr, STDERR_FILENO);

        close(fdout);
        close(fderr);

        execlp("file", "file", "--mime-type", "-f", TMP_BATCH, NULL);

        /**
         * If this code is executed it means that exec returned -1
         * and the errno variable was set
         */
        ERROR_CANT_EXECUTE_PROC();
    }

    waitpid(pid, NULL, 0);

#pragma endregion CHILD_PROCESS

    /*
     * ─────────────────────────────────────────────────── DISPLAY PROCESSED DATA ─────
     */

#pragma region DISPLAY

    if (!file_exists(TMP_SOUT))
    {
        ERROR_FILE_NOT_EXISTS(TMP_SOUT);
    }

    if (!is_regular_file(TMP_SOUT))
    {
        ERROR_INCORRECT_FILE_ARG(TMP_SOUT);
    }

    FILE *fl = fopen(TMP_SOUT, "r");
    if (fl == NULL)
    {
        ERROR_CANT_OPEN_FILE(TMP_SOUT);
    }

    ssize_t readed = 0;
    size_t lenght = 0;
    char *linebuffer = NULL;

    while ((readed = getline(&linebuffer, &lenght, fl)) != EOF)
    {
        // Confirm that the line has text
        if (strlen(linebuffer) > 0)
        {
            char file[MAX_QUEUE];
            char ext[MAX_QUEUE];
            char type[MAX_QUEUE];

            // Confirm that the line does not exceed the MAX_STRING_SIZE
            if (strlen(linebuffer) < (MAX_STRING_SIZE))
            {

                strcut(file,
                       linebuffer,
                       0,
                       strlen(linebuffer) - strlen(strchr(linebuffer, ' ')) - 2);

                current_file_in_batch = file;

                strcut(ext,
                       linebuffer,
                       0,
                       strlen(linebuffer) - strlen(strchr(linebuffer, ':')) - 1);

                char *pos = file_extension(ext);
                strcpy(ext, pos);

                strcut(type,
                       linebuffer,
                       strlen(linebuffer) - strlen(strrchr(linebuffer, ' ')) + 1,
                       strlen(linebuffer) - 2);

                strtolower(ext);
                strtolower(type);

                ON_DEBUG(MESSAGE_PROCESSING, "Checking file '%s' with (ext: %s) (type: %s)", file, ext, type);

                int ext_pos = array_has_string(supported_extensions, supported_extensions_count, ext);

                if (ext_pos < 0)
                {
                    counter_error++;
                    MESSAGE(MESSAGE_ERROR, "'%s': type '%s' is not supported", file, type);
                    continue;
                }

                if (strcmp(supported_types[ext_pos], type) == 0)
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

    // Summary display if
    if (display_summary)
    {
        printf("\n");
        MESSAGE(
            MESSAGE_SUMMARY,
            "\n\tFiles analyzed: %d\n\tFiles OK: %d\n\tFiles MISMATCH: %d\n\tErrors: %d",
            counter_analized, counter_ok, counter_mismatch, counter_error);
    }

    printf("\n");

#pragma endregion DISPLAY

// Cleanup memory
#ifndef SHOW_DEBUG
    unlink(TMP_SOUT);
    unlink(TMP_SERR);
    unlink(TMP_BATCH);
#endif
    fclose(fl);
    free(linebuffer);

    return EXIT_SUCCESS;
}

/*
 * ───────────────────────────────────────────────────────── HELPER FUNCTIONS ─────
 */

#pragma region HELPERS

void add_to_queue(char *string)
{
    strcpy(files_queue[queue_counter], string);
    queue_counter++;
    ON_DEBUG(MESSAGE_INFO, "Added new file to files_queue ( %s )", string);
}

void handle_signal(int signal, siginfo_t *info, void *context)
{
    (void)context;

    int aux = errno;

    switch (signal)
    {
    case SIGQUIT:
        printf("\n");
        MESSAGE(MESSAGE_SIGNAL, "\nCaptured %s (%s) signal (sent by PID: %ld). Use %s (%s) to terminate application.\n", "SIGQUIT", strsignal(SIGQUIT), info->si_pid, "SIGINT", strsignal(SIGINT));
        break;
    case SIGUSR1:;
        // Extra precaution
        if (batch_processing)
        {
            printf("\n");
            // fix for "labels": https://www.educative.io/edpresso/resolving-the-a-label-can-only-be-part-of-a-statement-error
            char formatted_time[MAX_STRING_SIZE];
            strftime(formatted_time, sizeof(formatted_time), "%Y.%m.%d_%Hh%M:%S", timeinfo);
            MESSAGE(MESSAGE_SIGNAL, "\n\t%s\n\tNº%d/%s\n", formatted_time, (counter_analized), (current_file_in_batch == NULL) ? "'File not retrieved yet'" : current_file_in_batch);
        }
        break;
    default:
        break;
    }

    errno = aux;
}

void argument_file()
{
    if (args.file_given)
    {
        MESSAGE(MESSAGE_INFO, "Analizing...");

        for (size_t i = 0; i < args.file_given; i++)
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
        }
    }
}

void argument_batch()
{
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
}

void argument_directory()
{
    if (args.dir_given)
    {
        display_summary = 1;
        if (!args.file_given && !args.batch_given)
        {
            MESSAGE(MESSAGE_INFO, "Analizing files listed in '%s' directory", args.dir_arg);
        }

        /*
            Checking if the last char is '/' or folder
            Windows not supported
        */
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
}

#pragma endregion HELPERS