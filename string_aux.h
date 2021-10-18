#ifndef _STRING_AUX_H_
#define _STRING_AUX_H_

void strcut(char *buffer, const char *string, int min, int max);

int array_has_string(const char **array, int size, const char *string);

void strtolower(char *string);

#endif