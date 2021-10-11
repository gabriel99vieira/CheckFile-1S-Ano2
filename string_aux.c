#include <stdio.h>
#include <string.h>
#include "string_aux.h"

void strcut(char *buffer, const char *string, int min, int max)
{
    int counter = 0;
    for (int i = min; i <= max; i++)
    {
        buffer[counter] = string[i];
        counter++;
    }
    buffer[counter] = '\0';
}

int array_has_string(const char **array, int size, const char *string)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(array[i], string) == 0)
        {
            return 1;
        }
    }

    return 0;
}