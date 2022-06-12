#include <string.h>
#include "myfs.h"
#include <fcntl.h>


int main() {
    mymkfs(30 * 1024);
    mymount("UFS", "", "", 0, NULL);

    { /// test 1
        int fd = myopen("/file1.txt", O_WRONLY | O_CREAT);
        mywrite(fd, "hello world", 11);
        myclose(fd);
        fd = myopen("/file1.txt", O_RDONLY);
        char buff[12];
        myread(fd, buff, 11);
        printf("%s\n", buff);
    }

    { /// test 2
        myclose(myopen("/foldi/file0.txt", O_CREAT));
        myclose(myopen("/foldi/file1.txt", O_CREAT));
        myclose(myopen("/foldi/file2.txt", O_CREAT));
        myclose(myopen("/foldi/file3.txt", O_CREAT));
        myclose(myopen("/foldi/file4.txt", O_CREAT));
        myclose(myopen("/foldi/file5.txt", O_CREAT));
        myclose(myopen("/nonsense/file0.txt", O_CREAT));
        myclose(myopen("/nonsense/file1.txt", O_CREAT));

        myDIR *dir = myopendir("/foldi");
        mydirent *dirent;
        while ((dirent = myreaddir(dir)) != NULL) {
            printf("%s\n", dirent->d_name);
        }
        myclosedir(dir);
    }
}
