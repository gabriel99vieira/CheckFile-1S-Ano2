#include "checkfile.h"

int checkfile(const char *file, const char **allowed, int (*check_extension)(const char *f))
{
    (void)file;
    (void)allowed;
    (void)check_extension;

    // for (int i = 0; i < (sizeof(allowed) / sizeof(*allowed)); i++)
    // {
    //     // printf("%s", allowed[i]);
    //     // if (strcmp(allowed[i], file) == 0)
    //     // {
    //     //     check_extension(file);
    //     //     return 1;
    //     // }
    // }
    return -1;
}

// int ck_pdf(const char *file)
// {
// }

// int ck_gif(const char *file)
// {
// }

// int ck_jpg(const char *file)
// {
// }

// int ck_png(const char *file)
// {
// }

// int ck_mp4(const char *file)
// {
// }

// int ck_zip(const char *file)
// {
// }

// int ck_html(const char *file)
// {
// }
