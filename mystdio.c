#include "mystdio.h"
#include "myfs.h"
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>


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


int myfscanf(myFILE *stream, const char *format, ...)
{
    /// read file to buffer
    int file_size = statSize(stream->fd);
    char* buf = (char*) malloc(file_size);
    myread(stream->fd, buf, file_size);

    const char* itr = format;
    va_list arg;
    va_start(arg, format);
    while(*itr != '\0')
    {
        if(*itr == '%')
        {
            char temp0 = *(++itr);
            if(temp0 == 'd')
            {
                int num = 0;
                while(*buf != ' '&& *buf != EOF &&*buf != '\n'&&*buf != '\0')
                {
                    num *=10;
                    num += (int)(*buf-'0');
                    ++buf;
                }

                int* ans;
                ans = va_arg(arg,int*);
                *ans = num;
                ++buf;
            }
            if(temp0 == 'c')
            {
                char* ans = va_arg(arg,char*);
                *ans = *buf;
                ++buf;
            }
            if(temp0 == 'f')
            {
                float val = 0;
                int afterdot=0;
                float scale=1;
                int neg = 0;

                if (*buf == '-') {
                    ++buf;
                    neg = -1;
                }
                float add = 0;
                while(*buf!= ' ' && *buf != EOF &&*buf != '\n'&&*buf != '\0')
                {
                    add = (*buf-'0');
                    if (afterdot)
                    {
                        scale = scale/10;
                        val = val + add*scale;
                    }
                    else
                    {
                        if (*buf == '.')
                        {
                            afterdot++;
                            ++buf;
                            continue;
                        }
                        val = val * 10.0 + add;
                    }
                    ++buf;
                }
                float* ans = va_arg(arg,float*);
                if(neg)
                    *ans = -val;
                else
                    *ans = val;
            }
        }
        ++itr;
    }
    return 0;
}


int myfprintf(myFILE *stream, const char *format, ...){
    char* buff;
    const char* itr = format;
    va_list arg;
    va_start(arg, format);
    while(*itr != '\0')
    {
        if(*itr == '%')
        {
            char temp = *(++itr);

            if (temp == 'd')
            {
                int temp0 = va_arg(arg,int);

                int size = (int)((floor(log10(temp0))+1)*sizeof(char));
                buff = (char*)malloc(size);
                sprintf(buff, "%d", temp0);
                for (int i = 0; i<size ;++i)
                {
                    myfwrite(&buff[i],1,1,stream);
                }
                free(buff);
            }
            if(temp == 'c')
            {
                char temp0 = (char )va_arg(arg,int);
                myfwrite(&temp0,sizeof (char ),1,stream);
            }
            if(temp == 'f')
            {
                double temp0 = va_arg(arg,double);
                float copy = temp0;

                int size = 0;
                int test = (int)copy;
                while(copy != (float)test)
                {
                    copy *= 10;
                    test = (int)copy;
                }
                while(copy>0)
                {
                    ++size;
                    copy/=10;
                }
                size++;

                buff = (char*)malloc(size);
                sprintf(buff, "%f", temp0);
                buff[size] = ' ';
                for (int i = 0; buff[i] !=' ' ;++i)
                {
                    myfwrite(&buff[i],sizeof (char),1,stream);
                }
                free(buff);
            }
        }
        ++itr;
    }
    return 0;
}
