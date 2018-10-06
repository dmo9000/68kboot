#include "stdio.h"
#include "fcntl.h"
#include "string.h"
#include "assert.h"
#include "errno.h"
#include "ext2.h"


int open(const char *pathname, int flags)
{
    uint32_t file_inode = 0;
    printf("open(%s, %u)\r\n", pathname, flags);
    assert(flags == O_RDONLY);

    file_inode = ext2_path_to_inode((char *) pathname);
    printf("file_inode = %lu\r\n", file_inode);

    if (file_inode == 0) {
            /* file not found */
            errno = ENOENT;
            return -1;
            }

    if (isdirectory(file_inode)) {
            errno = ENOTSUPP;
            return -1;
            }

    assert(NULL);
    return -1;

}

