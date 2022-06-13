#include "mystdio.h"
#include "myfs.h"
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>

int mode_to_flags(const char *restrict mode) {
    if (!strcmp(mode, "r")) {
        return O_RDONLY;
    }
    if (!strcmp(mode, "r+")) {
        return O_RDWR;
    }
    if (!strcmp(mode, "w")) {
        return O_WRONLY;
    }
    if (!strcmp(mode, "a")) {
        return O_APPEND;
    }
    return -1;
}

myFILE *myfopen(const char *restrict pathname, const char *restrict mode) {
    myFILE *file = malloc(sizeof(myFILE));
    int flags;
    if ((flags = mode_to_flags(mode) == -1)) {
        printf("don't support the mode %s", mode);
        exit(-1);
    }
    if ((file->fd = myopen(pathname, flags) == -1)) {
        printf("not enough Inodes %s", mode);
        exit(-1);
    }
    return file;
}

int myfclose(myFILE *stream) {
    myclose(stream->fd);
    free(stream);
    return 0;
}

size_t myfwrite(const void *restrict ptr, size_t size, size_t nmemb, myFILE *restrict stream) {
    return mywrite(stream->fd, ptr, size * nmemb);
}

int myfseek(myFILE *stream, long offset, int whence) {
    if (mylseek(stream->fd, offset, whence) == -1) {
        return -1;
    }
    return 0;
}

int count_percent(const char *restrict format) {
    int count = 0;
    for (int i = 0; i < strlen(format); ++i) {
        if (format[i] == '%') {
            count++;
        }
    }
    return count;
}

int myfscanf(myFILE *restrict stream, const char *restrict format, ...) {
    va_list args;
    int i = 0;
    int num_var = count_percent(format);
    va_start(args, num_var);
    while (i < num_var) {
        int j = 0;
        for (; j < strlen(format); ++j) {
            if (format[j] == '%') {
                ++j;
                break;
            }
        }
        if (format[j] == 'c') {
            myread(stream->fd, va_arg(args,int*),sizeof(char));
        }
        if (format[j] == 'f') {
            myread(stream->fd, va_arg(args,double*),sizeof(double));
        }
        if (format[j] == 'd') {
            myread(stream->fd, va_arg(args,int*),sizeof(int));
        }
        i++;
    }
    va_end(args);
    return i;
}

int myfprintf(myFILE *restrict stream, const char *restrict format, ...) {
    va_list args;
    int i = 0;
    int num_var = count_percent(format);
    va_start(args, num_var);
    while (i < num_var) {
        int j = 0;
        for (; j < strlen(format); ++j) {
            if (format[j] == '%') {
                ++j;
                break;
            }
        }
        if (format[j] == 'c') {
            myfwrite(va_arg(args,
            int*),sizeof(char), 1, stream);

        }
        if (format[j] == 'f') {
            myfwrite(va_arg(args,
            double*),sizeof(double), 1, stream);
        }
        if (format[j] == 'd') {
            myfwrite(va_arg(args,
            int*),sizeof(int), 1, stream);
        }
        i++;
    }
    va_end(args);
    return i;
}

