#ifndef UFS_OS_MYFS_H
#define UFS_OS_MYFS_H

#include <stdio.h>

#define BLOCK_SIZE 512
#define MAX_NAME 256

/**
 * @brief This file represent the header of our implementation for write, read, open, close, syscalls and dirent.h
 *
 * Here is the structure of one UFS.
 * The super block, inodes and block will first initial in every running of the program.
 * Also they will dynamically allocated in a static manner just because the assigment instructions.
 * Because of that there will be no use with pointers inside structs but only with indexes.
 */

typedef struct superBlock { // size = 16 bytes
    int inodesSize; // the number of inodes the ufs have
    int blocksSize; // the number of blocks the ufs have
    int freeInodes; // the number of free inodes the ufs have
    int freeBlocks; // the number of free blocks the ufs have
} superBlock;

typedef struct inode { // size = 260 bytes
    char name[MAX_NAME];
    int directedBlock; // index to the first block of the file (the blocks are in array)
    int bytesSize; // the size of bytes the file is taken in the data of the blocks
} inode;

typedef struct block { // size = 517
    int free: 1; // one bit to determine if the block is free
    int next_block; // pointer to the next block
    char data[BLOCK_SIZE]; // the data of the block
} block;

typedef struct fileDescriptor {
    int permissions;
    int offset;
    int inode; // the index of the file inode
} fileDescriptor;

typedef struct mydirent{
    int          inode;       // Inode index in the inodes list
    char         d_name[MAX_NAME]; // Null-terminated filename
} mydirent;

typedef struct myDIR{
    size_t size; // the number of entries in the directory
    size_t index; // the current entry
    mydirent *entry;
} myDIR;

void mymkfs(int size);

int mymount(const char *source, const char *target,const char *filesystemtype, unsigned long mountflags, const void *data);

int myopen(const char *pathname, int flags);

int myclose(int myfd);

ssize_t myread(int myfd, void *buf, size_t count);

ssize_t mywrite(int myfd, const void *buf, size_t count);

off_t mylseek(int myfd, off_t offset, int whence);

size_t statSize(int myfd);

myDIR *myopendir(const char *name);

mydirent *myreaddir(myDIR *dirp);

int myclosedir(myDIR *dirp);

#endif //UFS_OS_MYFS_H
