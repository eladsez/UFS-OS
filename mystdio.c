#include "mystdio.h"
#include "myfs.h"
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>


float mypow (float x, int y)
{
    float temp;
    if (y == 0)
        return 1;
    temp = mypow(x, y / 2);
    if ((y % 2) == 0) {
        return temp * temp;
    } else {
        if (y > 0)
            return x * temp * temp;
        else
            return (temp * temp) / x;
    }
}


int mode_to_flags(const char *restrict mode) {
    if (!strcmp(mode, "r")) {
        return O_RDONLY;
    }
    if (!strcmp(mode, "r+")) {

        return O_RDWR|O_CREAT;
    }
    if (!strcmp(mode, "w"))
    {
        return O_WRONLY|O_CREAT;
    }
    if (!strcmp(mode, "a")) {
        return O_APPEND;
    }
    return -1;
}

myFILE* myfopen(const char *restrict pathname, const char *restrict mode) {
    myFILE *file = malloc(sizeof(myFILE));
    int flags = mode_to_flags(mode);
    if (flags  == -1)
    {
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
size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream){
    size_t bytes_read = myread(stream->fd, (char *)ptr, size*nmemb);
    return bytes_read;
}

int myfseek(myFILE *stream, long offset, int whence) {
    if (mylseek(stream->fd, offset, whence) == -1) {
        return -1;
    }
    return 0;
}


int myfscanf(myFILE *stream, const char *format, ...){
    char* buf= malloc(sizeof(format));
    memset(buf, 0, sizeof(format));

    myfread(buf, 1, sizeof(format), stream);
    int i=0;
    int count = 0;
    float temp = 0;
    char* itr = format;
    va_list arg;
    va_start(arg, format);
    while(*itr != '\0')
    {
        if(*itr == '%')
        {
            char temp0 = *(++itr);
            if(temp0 == 'd')
            {
                temp0 =0;
                while(buf[i]!= ' '){
                    temp0 *=10;
                    temp0 += (float)(buf[i]-'0');
                    i++;
                }
                *(int*)arg = (int)temp;
                i++;
            }
            if(temp0 == 'c')
            {
                *(char*)arg = buf[i];
                i++;
            }
            if(temp0 == 'f')
            {
                temp = 0;
                while(buf[i]!= ' ' && buf[i] != '.'){
                    temp *=10;
                    temp += buf[i]-'0';
                    i++;
                }
                while(buf[i]!= ' ')
                {
                    float pow = mypow(10, count);
                    temp+=(float) ((buf[i]-'0')/pow);
                    count++;
                    i++;
                }
                *(float*)arg = temp;
            }
        }
        ++itr;
    }
    return 0;
}


int myfprintf(myFILE *stream, const char *format, ...){
    char* buf =  malloc(2*sizeof(format));;
    char* curr = malloc(2*sizeof (format));
    memset(buf, 0, sizeof(format));
    char* itr = format;
    va_list arg;
    va_start(arg, format);
    while(*itr != '\0')
    {
        if(*itr == '%')
        {
            char temp = *(++itr);
            if (temp=='d')
            {
                memset(curr, 0, sizeof(format));
                sprintf(curr, "%d", va_arg(arg,int));
                strcat(buf, curr);
                strcat(buf, " ");
            }
            if(temp == 'c')
            {
                char readed = (char)va_arg(arg, int);
                strncat(buf, &readed, 1);
                strcat(buf, " ");
            }
            if(temp == 'f')
            {
                memset(curr, 0, sizeof(format));
                sprintf(curr, "%f", va_arg(arg,double));
                strcat(buf, curr);
                strcat(buf, " ");
            }
        }
        ++itr;
    }
    myfwrite(buf, 1, sizeof(buf), stream);
    return 0;
}
