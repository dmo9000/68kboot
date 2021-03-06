#define __BDOS__

#include "stdio.h"
#include <string.h>
#include "fcntl.h"
#include "errno.h"
#include "assert.h"
#include "byteorder.h"

_fd file_descriptor[MAX_FDS];

extern ext2_fs ext2_rootfs;
bool file_table_initialized = false;


void initialize_file_table()
{
    int i = 0;

    for (i = 0; i < MAX_FDS; i++) {
        file_descriptor[i].state = FD_STATE_UNUSED;
    }

    file_descriptor[0].state = FD_STATE_STDIN;
    file_descriptor[1].state = FD_STATE_STDOUT;
    file_descriptor[2].state = FD_STATE_STDERR;

    file_table_initialized = true;
    return;
}

int fcntl_find_free_fd()
{

    int i = 0;

    if (!file_table_initialized) {
        initialize_file_table();
    }

    for (i = 0; i < MAX_FDS; i++) {
        if (file_descriptor[i].state  == FD_STATE_UNUSED) {
            return i;
        }
    }
    return -1;

}

int fcntl_new_inode()
{
    uint32_t new_inode = 0;
    bool new_inode_state = false;
    if (!file_table_initialized) {
        initialize_file_table();
    }
    new_inode = ext2_next_free_inode(&ext2_rootfs);
    new_inode_state = ext2_get_inode_bitmap_state(&ext2_rootfs, new_inode );
    //kprintf("inode(%lu) state = %s\n\r", new_inode, (new_inode_state ? "true" : "false"));

    if (new_inode_state == true) {
        kprintf("+++ bitmap query says inode is allocated!!\n\r");
        set_errno(EIO);
        return -1;
    }

    /* set the inode bit to true */

    ext2_set_inode_bitmap_state(&ext2_rootfs, new_inode, true);

    set_errno(EIO);
    return -1;
}

int fcntl_open_inode(uint32_t inode, int flags)
{
    int new_fd = -1;

    if (!ext2_inode_lookup(inode, &file_descriptor[new_fd].fd_inode, false)) {
        /* file does not exist */
        set_errno(ENOENT);
        return -1;
    }


    new_fd = fcntl_find_free_fd();

    if (new_fd == -1) {
        set_errno(EMFILE);
        return -1;
    }

    assert(ext2_inode_lookup(inode, &file_descriptor[new_fd].fd_inode, false));

    /* FIXME: we should come up some compile time defines for DIRECT, INDIRECT, INDIRECT2, INDIRECT3 etc */

    if (!(nm_uint32(file_descriptor[new_fd].fd_inode.i_size) <= ((EXT2_NDIR_BLOCKS * ext2_rootfs.block_size) + (EXT2_IND_BLOCKS* ext2_rootfs.block_size)
            + (EXT2_IND_BLOCKS * ext2_rootfs.block_size * EXT2_IND_BLOCKS)
                                                                ))) {
        // quick check for now - we can only handle files with direct and indirect blocks, anything else is too large
        // kprintf("FILE TOO BIG!\r\n");
        set_errno(EFBIG);
        // kprintf("returning errno = %d\r\n", errno);
        return -1;
    }

    file_descriptor[new_fd].state = FD_STATE_OPEN;
    file_descriptor[new_fd].inode = inode;
    /* TODO: check flags validity here */
    file_descriptor[new_fd].flags = flags;
    /* position at start of file */
    file_descriptor[new_fd].offset = 0;
    file_descriptor[new_fd].fs = &ext2_rootfs;
    return new_fd;
}

int fcntl_close(int fildes)
{


    //printf("fcntl_close(%d)\r\n", fildes);
    if (fildes < 0 || fildes >= MAX_FDS) {
        set_errno(EBADF);
        return -1;
    }

    if (file_descriptor[fildes].state != FD_STATE_OPEN) {
        set_errno(EBADF);
        return -1;
    }

    file_descriptor[fildes].state = FD_STATE_UNUSED;
    file_descriptor[fildes].inode = 0;
    file_descriptor[fildes].offset = 0;
    kernel_memset(&file_descriptor[fildes].fd_inode, 0, sizeof(ext2_inode));
    set_errno(0);
    return 0;

}

_fd *fcntl_get_descriptor(int fd)
{

//    kprintf("my->fd = %d\r\n", fd);
    assert(!(fd < 0 || fd >= MAX_FDS));
    return (_fd *) &file_descriptor[fd];
}


int kfcntl(int fd, int cmd, int tf)
{

    assert(!(fd < 0 || fd >= MAX_FDS));
    switch(cmd) {
    case F_GETFL:
        //printf("kfcntl(%d, F_GETFL, %d)\n", fd, tf);
        return file_descriptor[fd].tflags;
        break;
    case F_SETFL:
        //printf("kfcntl(%d, F_SETFL, %d)\n", fd, tf);
        file_descriptor[fd].tflags = tf;
        return 0;
        break;
    default:
        kprintf("kfcntl(%d, <UNHANDLED>, %d)\n", fd, tf);
        break;
    }
    return 0;
}
