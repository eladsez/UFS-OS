#include <string.h>
#include "myfs.h"
#include <fcntl.h>


int main() {
    mymkfs(30 * 1024);
    mymount("UFS", "", "", 0, NULL);
    char buff[1024];

    {
        int fd;
//        /// tests for the basic system calls operations: myopen, myclose, mywrite, myread, mylseek
        memset(buff, 0, 1024);
//        fd = myopen("/file1.txt", O_WRONLY | O_CREAT);
//        mywrite(fd, "hello world", 11);
//        myclose(fd);
//        fd = myopen("/file1.txt", O_RDONLY);
//        myread(fd, buff, 11);
//        printf("%s\n\n", buff); // should print hello world
//        myclose(fd);

        fd = myopen("/file2.txt", O_RDWR | O_CREAT);
        mywrite(fd, "it's the first sentences!", 25);
        mylseek(fd, 0, SEEK_SET);
        myread(fd, buff, 25);
        printf("%s\n", buff); // should print it's the first sentences

        mywrite(fd, " it's the second sentences!", 27);
        mylseek(fd, -26, SEEK_END);
        myread(fd, buff, 26);
        printf("%s\n", buff); // should print: "it's the second sentences\n
        myclose(fd);

        /// know tests for the append mode: mywrite, mylseek
        fd = myopen("/file2.txt", O_APPEND);
        mywrite(fd, " i'm appending! its will not run over the latest 2 sentences.", 66);
        myclose(fd);

        fd = myopen("/file2.txt", O_RDWR);
        printf("\nprint everything from file2.txt (except \"hello world\") with the appending: \n\n");
        myread(fd, buff, 121);
        printf("%s\n", buff); // should print: "it's the first sentences\nknow i'm appending! its will not run over the latest 2 sentences
    }



    {
        /// test 2
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
        // should print all the files in the foldi folder
        while ((dirent = myreaddir(dir)) != NULL) {
            printf("%s\n", dirent->d_name);
        }
        myclosedir(dir);
    }
}
