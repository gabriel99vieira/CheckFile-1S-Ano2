#ifndef _STRING_AUX_H_
#define _STRING_AUX_H_

/**
 * @brief Cuts a string between two positions
 *
 * @param buffer
 * @param string
 * @param min
 * @param max
 */
void strcut(char *buffer, const char *string, int min, int max);

/**
 * @brief Search the occurence in the array returning its position
 *
 * @param array
 * @param size
 * @param string
 * @return int
 */
int array_has_string(const char **array, int size, const char *string);

/**
 * @brief Switches all the upper case characters in a string to its matching lower case
 *
 * @param string
 */
void strtolower(char *string);

#endif