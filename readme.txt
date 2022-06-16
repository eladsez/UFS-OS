                                        *-FILES INFORMATION-*
the first section of the assigment was implemention of simple ufs which we implemented in myfs.c
we implemented the ufs by implementing the the following functions:
mymount
myopen
myclose
myread
mywrite
mylseek
myopendir
myreaddir
myclosedir

the secont section of the assigment was implemention of our version of libc which we implemented in mylib.c
we implemented the library by implementing the the following functions:
myfopen
myfclose
myfread
myfwrite
myfseek
myfscanf
myfprintf


We tested all of the above functions in main.c file, main file is well documented for every test.

                          *-HOW TO RUN-*
First open a terminal in the UFS-OS-main directory and type <make all>.
Now after everything compiled you can run the following targets:
a) To run the tests for the assigment type <./main>

                              *-EXTRA-*
All our source code are compiled to a shared library and our tests use this shared library you can see it in the makefile.
