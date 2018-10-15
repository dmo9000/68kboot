#include "stdio.h"
#include "string.h"
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

int fcntl_open_inode(uint32_t inode, int flags)
{
    int new_fd = -1;

    new_fd = fcntl_find_free_fd();

    if (new_fd == -1) {
        errno = EMFILE;
        return -1;
    }

    assert(ext2_inode_lookup(inode, &file_descriptor[new_fd].fd_inode, false));
    /*
    if (!(nm_uint32(file_descriptor[new_fd].fd_inode.i_size) <= (EXT2_NDIR_BLOCKS * ext2_rootfs.block_size))) {
        // quick check for now - we can only handle files with direct blocks, anything else is too large 
        errno = EFBIG;
        return -1;
    }
    */

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
        errno = EBADF;
        return -1;
    }

    if (file_descriptor[fildes].state != FD_STATE_OPEN) {
        errno = EBADF;
        return -1;
    }

    file_descriptor[fildes].state = FD_STATE_UNUSED;
    file_descriptor[fildes].inode = 0;
    file_descriptor[fildes].offset = 0;
    memset(&file_descriptor[fildes].fd_inode, 0, sizeof(ext2_inode));
    errno = 0;
    return 0;

}

_fd *fcntl_get_descriptor(int fd)
{

//    printf("my->fd = %d\r\n", fd);
    assert(!(fd < 0 || fd >= MAX_FDS));
    return (_fd *) &file_descriptor[fd];
}
