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

int check(const char *type, const char *extension)
{
    if (strcmp(extension, "jpg") == 0)
    {
        return strcmp(type, "jpeg") == 0 ? 1 : 0;
    }

    if (strcmp(extension, "txt") == 0)
    {
        return strcmp(type, "plain") == 0 ? 1 : 0;
    }

    return strcmp(type, extension) == 0 ? 1 : 0;
}