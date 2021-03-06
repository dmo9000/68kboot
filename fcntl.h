#ifndef __FCNTL_H__
#define __FCNTL_H__
#include "ext2.h"

#define O_RDONLY        00
#define O_WRONLY        01
#define O_TRUNC     01000
#define O_RDWR         02

#define MAX_FDS     16
#define FD_STATE_UNUSED    -1
#define FD_STATE_STDIN      0
#define FD_STATE_STDOUT     1
#define FD_STATE_STDERR     2
#define FD_STATE_OPEN       10

typedef struct {
    int id;
    int oflags;
    /* FIXME: should be off_t ? */
    uint32_t offset;
//    FCB fcb;
    int fd;
} _cfd;

typedef struct _fd {
    char state;
    int flags;
    uint32_t inode;
    uint32_t offset;
    uint32_t limit;
    ext2_inode fd_inode;
    ext2_fs *fs;
    int tflags;
} _fd;

int open(const char *pathname, int flags);


int fcntl_find_free_fd();
int fcntl_open_inode(uint32_t inode, int flags);
int fcntl_close(int fildes);
_fd *fcntl_get_descriptor(int fd);


#endif /* __FCNTL_H__ */
