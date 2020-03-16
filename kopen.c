#define __BDOS__

#include "stdio.h"
#include "fcntl.h"
#include <string.h>
#include "assert.h"
#include "errno.h"
#include "ext2.h"
#include "sys/stat.h"

extern ext2_fs ext2_rootfs;

int kopen(const char *pathname, int flags)
{
    uint32_t file_inode = 0;
    int new_fd = 0;
    //printf("open(%s, %u)\r\n", pathname, flags);
    //assert(flags == O_RDONLY);

    file_inode = ext2_path_to_inode((char *) pathname, ext2_rootfs.cwd_inode);
    //printf("file_inode = %lu\r\n", file_inode);

    if (file_inode == 0) {
        /* file not found */

        if (flags & O_CREAT) {
            /* check for free inode and free block */

            if (nm_uint32(ext2_rootfs.blck.s_free_inodes_count) &&
                    nm_uint32(ext2_rootfs.blck.s_inodes_count)) {
                file_inode = fcntl_new_inode();
                if (!file_inode) {
                    kprintf("kopen: create flag failed\n\r");
                    while (1) { }
                    return -1;
                }
                //kprintf("kopen: create flag okay\n\r");
                set_errno(EIO);
                return -1;
            }

            /* write was requested, should create file - but not supported yet */
            set_errno(EPERM);
            return -1;
        }

        if (!errno) {
            /* catch all, if errno not already set */
            set_errno(ENOENT);
        }
        return -1;
    }


    /* directories are files ! */

    /*
    if (isdirectory(file_inode)) {
    set_errno(EISDIR);
    return -1;
    }
    */

    new_fd= fcntl_open_inode(file_inode, flags);
    if (new_fd == -1) {
        //printf("funky! new_fd = %d, errno = %d\r\n", new_fd, errno);
        return -1;
    }
    //printf("fd = %d\r\n", new_fd);

    set_errno(0);
    return new_fd;

}

