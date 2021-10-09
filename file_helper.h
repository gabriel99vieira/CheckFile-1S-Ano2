#ifndef _FILE_HELPER_H_
#define _FILE_HELPER_H_

int file_exists(const char *filename);

int create_file(const char *filename, mode_t mode);

int trunc_file(const char *filename);

int open_file(const char *filename, int mode);

off_t file_size(int fd);

int is_directory(const char *path);

int is_regular_file(const char *path);

char *file_extension(const char *file);

#endif