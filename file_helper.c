#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "debug.h"
#include "message.h"
#include "file_helper.h"

off_t file_size(int fd)
{
    int offset = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);
    return offset;
}

int open_file(const char *filename, int mode)
{
    if (!file_exists(filename))
    {
        ERROR(EXIT_FAILURE, "'%s' file doesn't exist\n", filename);
    }
    int fd = open(filename, mode);
    if (fd == -1)
    {
        close(fd);
        ERROR(EXIT_FAILURE, "Could not open '%s'\n", filename);
    }
    return fd;
}

int file_exists(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    close(fd);
    return (fd == -1) ? 0 : 1;
}

int create_file(const char *filename, mode_t mode)
{
    if (mode == 0)
    {
        mode = 644;
    }

    int fd = -1;
    if (!file_exists(filename))
    {
        fd = creat(filename, mode);
    }

    return fd;
}

int trunc_file(const char *filename)
{
    return creat(filename, 0);
}

int is_directory(const char *path)
{
    struct stat f_stat;

    if (stat(path, &f_stat) == 0)
    {
        return S_ISDIR(f_stat.st_mode);
    }

    return 0;
}

int is_regular_file(const char *path)
{
    struct stat f_stat;

    if (stat(path, &f_stat) == 0)
    {
        return S_ISREG(f_stat.st_mode);
    }

    return 0;
}

char *file_extension(const char *file)
{
    char *pos = strchr(file, '.');
    return (pos == NULL) ? "" : (pos + 1);
}