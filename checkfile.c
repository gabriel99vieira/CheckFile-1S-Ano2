#include <stdio.h>
#include <string.h>
#include "checkfile.h"

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