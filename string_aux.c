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