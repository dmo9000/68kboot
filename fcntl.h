#ifndef __FCNTL_H__
#define __FCNTL_H__
#include "ext2.h"

#define O_RDONLY    0

#define MAX_FDS     16
#define FD_STATE_UNUSED    -1
#define FD_STATE_STDIN      0 
#define FD_STATE_STDOUT     1 
#define FD_STATE_STDERR     2
#define FD_STATE_OPEN       10


typedef struct _fd {
                    char state;
                    int flags;
                    uint32_t inode;
                    uint32_t offset; 
                    uint32_t limit; 
                   } _fd;

int open(const char *pathname, int flags);


int fcntl_find_free_fd();
int fcntl_open_inode(uint32_t inode, int flags);

#endif /* __FCNTL_H__ */
