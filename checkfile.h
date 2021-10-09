#ifndef _CHECK_FILE_H_
#define _CHECK_FILE_H_

int checkfile(const char *file, const char **allowed, int (*check_extension)(const char *f));

int ck_pdf(const char *file);
int ck_gif(const char *file);
int ck_jpg(const char *file);
int ck_png(const char *file);
int ck_mp4(const char *file);
int ck_zip(const char *file);
int ck_html(const char *file);

#endif