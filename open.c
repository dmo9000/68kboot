#include "stdio.h"
#include "fcntl.h"
#include "string.h"
#include "assert.h"
#include "errno.h"


int open(const char *pathname, int flags)
{
    uint32_t file_inode = 0;
    int new_fd = 0;
    //printf("open(%s, %u)\r\n", pathname, flags);
    assert(flags == O_RDONLY);

    file_inode = ext2_path_to_inode((char *) pathname);
    //printf("file_inode = %lu\r\n", file_inode);

    if (file_inode == 0) {
        /* file not found */
        if (!errno) {
            /* catch all, if errno not already set */
            errno = ENOENT;
        }
        return -1;
    }

    if (isdirectory(file_inode)) {
        errno = EISDIR;
        return -1;
    }

    new_fd= fcntl_open_inode(file_inode, flags);
    if (new_fd == -1) {
        return -1;
    }
    //printf("fd = %d\r\n", new_fd);


    errno = 0;
    return new_fd;

}

