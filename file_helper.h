#ifndef _FILE_HELPER_H_
#define _FILE_HELPER_H_

/**
 * @brief Checks if a the file exists
 *
 * @param filename
 * @return int
 */
int file_exists(const char *filename);

/**
 * @brief Create a file object  (file descriptor is returned on success)
 *
 * @param filename
 * @param mode
 * @return int
 */
int create_file(const char *filename, mode_t mode);

/**
 * @brief Creates a file or overrides the existing one without content (file descriptor is returned on success)
 *
 * @param filename
 * @return int
 */
int trunc_file(const char *filename);

/**
 * @brief Opens a file and returns its descripto on success
 *
 * @param filename
 * @param mode
 * @return int
 */
int open_file(const char *filename, int mode);

/**
 * @brief Returns the file size in bytes
 *
 * @param fd
 * @return off_t
 */
off_t file_size(int fd);

/**
 * @brief Checks if given path is a directory
 *
 * @param path
 * @return int
 */
int is_directory(const char *path);

/**
 * @brief Checks if given path is a regular file
 *
 * @param path
 * @return int
 */
int is_regular_file(const char *path);

/**
 * @brief Gets a pointer to the start of the extension from the given file string (TEXT.EXT)
 *
 * @param file
 * @return char*
 */
char *file_extension(const char *file);

#endif