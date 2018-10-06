#ifndef __FCNTL_H__
#define __FCNTL_H__
#include "ext2.h"

#define O_RDONLY    0

#define MAX_FDS     16
#define FD_STATE_UNUSED     -1


typedef struct _fd {
                    uint8_t state;
                    ext2_inode inode;
                    int flags;
                   } _fd;

int open(const char *pathname, int flags);

#endif /* __FCNTL_H__ */
