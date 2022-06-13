#include "myfs.h"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

superBlock superb;
inode *inodes;
block *blocks;
fileDescriptor fd[10000]; // the same as the instruction

/**
 * update the file system into his file
 * @param path - the path to the ufs file (ufs hard disk so called)
 */
void syncUFS(const char *path) {
    char *ufs;
    size_t totalSize = sizeof(superBlock) + superb.inodesSize + superb.blocksSize + 1;

    int ufs_fd = open(path, O_RDWR | O_CREAT, (mode_t) 0600);
    if (ufs_fd == -1) {
        printf("ERROR: failed to open ufs file\n");
        exit(1);
    }

    /// need to find something better then that as a solution
    lseek(ufs_fd, (int) totalSize - 1, SEEK_SET);
    write(ufs_fd, "", 1);

    ufs = (char *) mmap(0, totalSize, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FILE, ufs_fd, 0);
    if (ufs == MAP_FAILED) {
        printf("ERROR: mmap failed map ufs file\n");
        exit(1);
    }
    memcpy(ufs, &superb, sizeof(superBlock)); // copy the super block to the ufs-file
    memcpy(ufs + sizeof(superBlock), inodes, sizeof(inode) * superb.inodesSize); // copy the super block to the ufs-file
    memcpy(ufs + sizeof(superBlock) + superb.inodesSize, blocks,
           superb.blocksSize); // copy the super block to the ufs-file

    munmap(ufs, totalSize); // free the memory of the file from the program
    close(ufs_fd); // we have to use close here if we want to open it elsewhere on the program (and we want)
}

/**
 * create a new file system
 * @param size - the size of the file system in bytes
 */
void mymkfs(int size) {

    // the following computing is because of the assigment instructions

    superb.inodesSize = (int) (size / 10 / sizeof(inode)); // 10% from size
    superb.blocksSize = (int) (size - superb.inodesSize -
                               16 / sizeof(block)); // the rest of size except the super block and inodes
    superb.freeInodes = superb.inodesSize;
    superb.freeBlocks = superb.blocksSize;

    int i;
    inodes = malloc(sizeof(inode) * superb.inodesSize);
    for (i = 0; i < superb.inodesSize; ++i) {
        inodes[i].directedBlock = -1;
        inodes[i].bytesSize = 0;
    }

    blocks = malloc(sizeof(block) * superb.blocksSize);
    for (i = 0; i < superb.blocksSize; ++i) {
        blocks[i].next_block = -1;
        blocks[i].free = 1;
    }

    if (blocks == NULL || inodes == NULL) {
        printf("ERROR: malloc error in UFS creation\n");
        exit(1);
    }

    for (i = 0; i < 10000; ++i) {
        fd[i].inode = -1;
        fd[i].offset = -1;
        fd[i].permissions = -1;
    }
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
            const void *data) {
    syncUFS(source);
    return 0;
}

/**
 * Function for finding new inodes
 * @return the index of the new inode in the inodes list
 */
int findNewInode() {
    for (int i = 0; i < superb.inodesSize; ++i) {
        if (inodes[i].directedBlock == -1) {
            return i;
        }
    }
    return -1;
}

/**
 * Function for finding new free block
 * @return the index of the new block in the blocks list
 */
int findNewBlock() {
    for (int i = 0; i < superb.blocksSize; ++i) {
        if (blocks[i].free) {
            return i;
        }
    }
    return -1;
}

/**
 * Function for finding new free fd
 * @return the index of the new fd in the fd list
 */
int findNewFD() {
    for (int i = 0; i < 10000; ++i) {
        if (fd[i].inode == -1) {
            return i;
        }
    }
    return -1;
}

/**
 * Function for finding inode by his name
 * @return the index of the inode in the inodes list
 */
int findInode(const char *name) {
    for (int i = 0; i < superb.inodesSize; ++i) {
        if (!strcmp(inodes[i].name, name)) {
            return i;
        }
    }
    return -1;
}

/**
 * Creating a new file allocate inode and one block for it
 * @param name - the name of the file
 * @return the index of the new inode in the inode list
 */
int createNewFile(const char *name) {
    int inode = findNewInode();
    int block = findNewBlock();
    if (inode == -1 || block == -1) {
        return -1;
    }

    strcpy(inodes[inode].name, name);
    inodes[inode].directedBlock = block;
    blocks[block].free = 0;

    return inode;
}

/**
 * My implementation for the open syscall
 * @param pathname - the path of the file we desire to open
 * @param flags - permissions to the file
 * @return in successes return new fd to the opened file, otherwise -1
 */
int myopen(const char *pathname, int flags) {
    int inode = findInode(pathname);

    if (inode == -1 && flags & O_CREAT)
    { // if the file doesn't exist and O_CREAT so create it

        inode = createNewFile(pathname);
    }
    if (inode == -1) { // if the file doesn't exist and no O_CREAT
        return -1;
    }
    int newfd = findNewFD();
    if (newfd == -1) { // if the fd list is full return -1
        return -1;
    }
    if ((flags & O_APPEND) == O_APPEND) {
        fd[newfd].offset = inodes[inode].bytesSize;
    } else {
        fd[newfd].offset = 0;
    }

    fd[newfd].inode = inode;
    fd[newfd].permissions = flags;
    return newfd;
}

/**
 * My implementation for the close syscall
 * @param myfd - the file descriptor we wish to close
 * @return - 0 (currently)
 */
int myclose(int myfd) {
    fd[myfd].inode = -1;
    fd[myfd].permissions = -1;
    fd[myfd].offset = -1;
    return 0;
}

/**
 * My implementation for the read syscall
 * @param myfd - the file descriptor we wish to read from
 * @param buf - the buffer we read into
 * @param count how much to read (max data to read)
 * @return in successes return the size we actually read, otherwise -1
 */
ssize_t myread(int myfd, void *buf, size_t count) {
    // if it doesn't have permission to read or the fd doesn't exist return error
    if ((fd[myfd].permissions != (O_RDONLY | O_CREAT) && fd[myfd].permissions != (O_RDWR | O_CREAT) &&
         fd[myfd].permissions != O_RDONLY && fd[myfd].permissions != O_RDWR) || fd[myfd].inode == -1) {
        return -1;
    }
    ssize_t dataSum = 0;
    int block = inodes[fd[myfd].inode].directedBlock;
    int tempOffset = fd[myfd].offset;
    while (tempOffset > BLOCK_SIZE) { // finding the block we need to read from helping the offset of myfd
        block = blocks[block].next_block;
        tempOffset -= BLOCK_SIZE;
    }

    // read until we finish the file or until count is smaller than a full block size
    while (block != -1 && count >= BLOCK_SIZE) {

        fd[myfd].offset += BLOCK_SIZE - tempOffset; // increase the actual offset
        dataSum += BLOCK_SIZE - tempOffset;
        strncpy(buf, blocks[block].data + tempOffset, BLOCK_SIZE - tempOffset);
        tempOffset = 0;
        block = blocks[block].next_block;
        count -= BLOCK_SIZE;
    }
    if (block != -1) { // if the block not over it means the count < block size so read the rest
        strncpy(buf, (blocks[block].data + tempOffset), count);
        dataSum += (ssize_t) count;
    }
    fd[myfd].offset += (int) dataSum;
    return dataSum;
}

/**
 * My implementation to write syscall
 * @param myfd - the file descriptor we want to writ to
 * @param buf - what to write
 * @param count - how much to write
 * @return the sum bytes writen
 */
ssize_t mywrite(int myfd, const void *buf, size_t count) {
    // if it doesn't have permission to write or the fd doesn't exist return error
    if ((fd[myfd].permissions != (O_WRONLY | O_CREAT) && fd[myfd].permissions != (O_RDWR | O_CREAT) &&
         fd[myfd].permissions != O_WRONLY && fd[myfd].permissions != O_RDWR &&
         fd[myfd].permissions != (O_APPEND | O_CREAT) && fd->permissions != O_APPEND) || fd[myfd].inode == -1) {
        return -1;
    }

    ssize_t dataSum = 0;
    int block = inodes[fd[myfd].inode].directedBlock;
    int tempOffset = fd[myfd].offset;

    while (tempOffset > BLOCK_SIZE) { // finding the block we need to write to with helping from the offset of myfd
        block = blocks[block].next_block;
        tempOffset -= BLOCK_SIZE;
    }

    // write until we reach the last block or until count is smaller than a full block size
    while (blocks[block].next_block != -1 && count >= BLOCK_SIZE) {
        strncpy(blocks[block].data + tempOffset, buf, BLOCK_SIZE - tempOffset);
        tempOffset = 0;
        block = blocks[block].next_block;
        count -= BLOCK_SIZE;
        dataSum += BLOCK_SIZE;
    }

    // this loop is when we need to creat new blocks
    while (blocks[block].next_block == -1 && count >= BLOCK_SIZE) {
        strncpy(blocks[block].data + tempOffset, buf, BLOCK_SIZE - tempOffset);
        tempOffset = 0;
        count -= BLOCK_SIZE;
        dataSum += BLOCK_SIZE;
        // finding a new block to write on
        blocks[block].next_block = findNewBlock();
        if (blocks[block].next_block == -1) {
            return -1;
        }
        blocks[blocks[block].next_block].free = 0;
        block = blocks[block].next_block;
    }

    // this loop is when we need to create new blocks
    while (blocks[block].next_block == -1 && count < BLOCK_SIZE && count > 0) {
        if (count + tempOffset < BLOCK_SIZE) {
            strncpy(blocks[block].data + tempOffset, buf, count);
            dataSum += (int) count;
            count = 0;
        } else {
            strncpy(blocks[block].data + tempOffset, buf, BLOCK_SIZE - tempOffset);
            tempOffset = 0;
            count -= BLOCK_SIZE + tempOffset;
            dataSum += BLOCK_SIZE - tempOffset;
            // finding a new block to write on
            blocks[block].next_block = findNewBlock();
            if (blocks[block].next_block == -1) {
                return -1;
            }
            blocks[blocks[block].next_block].free = 0;
            block = blocks[block].next_block;
        }
    }
    fd[myfd].offset += (int)dataSum;
    inodes[fd[myfd].inode].bytesSize += (int) dataSum;
    return dataSum;
}

off_t mylseek(int myfd, off_t offset, int whence) {
    if ((fd[myfd].permissions & O_APPEND) == O_APPEND) { // can't seek if we are in append mode
        return -1;
    }
    if (whence == SEEK_SET) {
        fd[myfd].offset = (int) offset;
    }
    if (whence == SEEK_CUR) {
        fd[myfd].offset += (int) offset;
    }
    if (whence == SEEK_END) {
        fd[myfd].offset = inodes[fd[myfd].inode].bytesSize + (int) offset;
    }
    return fd[myfd].offset;
}

myDIR *myopendir(const char *name) {
    int childs[superb.inodesSize];
    size_t entries = 0;
    for (int i = 0; i < superb.inodesSize; ++i) {
        char *poten_child = inodes[i].name;
        int j;
        for (j = (int) strlen(poten_child) - 1; j > 0; --j) {
            if (poten_child[j] == '/') {
                break;
            }
        }
        if (j == 0 && strlen(name) != 1) continue;
        if (j != 0 && strlen(name) == 1) continue;
        int minlen = (strlen(name) >= j) ? j : (int) strlen(name);
        if (strncmp(name, poten_child, minlen) != 0) {
            continue;
        }
        childs[entries] = i;
        ++entries;

    }
    myDIR *ans = malloc(sizeof(myDIR));
    ans->entry = malloc((entries + 1) * sizeof(struct mydirent));
    for (int i = 0; i < entries; ++i) {
        ans->entry[i].inode = childs[i];
        strcpy(ans->entry[i].d_name, inodes[childs[i]].name);
    }
    ans->size = entries;
    ans->index = 0;
    return ans;
}

mydirent *myreaddir(myDIR *dirp) {
    if (dirp->size == dirp->index) {
        return NULL;
    }
    mydirent *ans = &(dirp->entry[dirp->index]);
    dirp->index += 1;
    return ans;
}

int myclosedir(myDIR *dirp) {
    free(dirp->entry);
    free(dirp);
    return 0;
}

