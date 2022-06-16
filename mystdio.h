#ifndef UFS_OS_MYSTDIO_H
#define UFS_OS_MYSTDIO_H
#include <stdio.h>

typedef struct myFILE {
    int fd;
} myFILE;

myFILE *myfopen(const char *restrict pathname, const char *restrict mode);

int myfclose(myFILE *stream);

size_t myfwrite(const void *restrict ptr, size_t size, size_t nmemb, myFILE *restrict stream);

size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream);

int myfseek(myFILE *stream, long offset, int whence);

int myfscanf(myFILE *restrict stream, const char *restrict format, ...);

int myfprintf(myFILE *restrict stream, const char *restrict format, ...);

#endif // UFS_OS_MYSTDIO_H