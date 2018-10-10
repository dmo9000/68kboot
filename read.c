#include "stdio.h"
#include "stddef.h"
#include "types.h"
#include "fcntl.h"
#include "assert.h"
#include "errno.h"


ssize_t read(int fd, void *buf, size_t count)
{
    
    uint32_t block_id = 0;
    uint32_t block_offset = 0;
    //uint32_t remaining = count;
   // uint32_t bytes_read = 0;
    _fd *descriptor = fcntl_get_descriptor(fd);

    assert(descriptor);

    if (descriptor->offset >= (EXT2_NDIR_BLOCKS * descriptor->fs->block_size)) {
        printf("read() - offset %lu is out of range\r\n", descriptor->offset);
        errno = EIO;
        return -1;
        }

    block_id = descriptor->offset / descriptor->fs->block_size;
    block_offset = descriptor->offset % descriptor->fs->block_size; 
    printf("[ start block is within range -> block %lu, offset %lu]\r\n", block_id, block_offset);

    errno = EIO;
    return -1;
}
