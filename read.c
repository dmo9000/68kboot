#include "stdio.h"
#include "stddef.h"
#include "types.h"
#include "fcntl.h"
#include "assert.h"
#include "errno.h"
#include "byteorder.h"

extern _device devices[MAX_DEVICES];


ssize_t read(int fd, void *buf, size_t count)
{

    uint32_t file_block_id = 0;
    uint32_t file_block_offset = 0;
    uint32_t fs_block_id = 0;

    uint32_t remaining = count;
    uint32_t bytes_to_read = 0;

    uint32_t total_available = 0;
    uint32_t bytes_available = 0;
    uint32_t total_size = 0;
    // uint32_t bytes_read = 0;
    _fd *descriptor = fcntl_get_descriptor(fd);

    assert(descriptor);

    if (descriptor->offset >= (EXT2_NDIR_BLOCKS * descriptor->fs->block_size)) {
        printf("read() - offset %lu is out of range\r\n", descriptor->offset);
        errno = EIO;
        return -1;
    }

    file_block_id = descriptor->offset / descriptor->fs->block_size;
    file_block_offset = descriptor->offset % descriptor->fs->block_size;
    printf("[ start block is within range -> block %lu, offset %lu]\r\n", file_block_id, file_block_offset);

    fs_block_id = ext2_get_inode_block(&descriptor->fd_inode, file_block_id);
    printf("target offset = (0x%08lx+0x%08lx:0x%08lx+0x%08lx)\r\n", fs_block_id, file_block_offset, (
               fs_block_id * descriptor->fs->block_size), file_block_offset);

    //assert(ext2_block_read(descriptor->fs, 0xE000, fs_block_id));
   
    /* total available bytes in file, from the seek position */
    total_available = nm_uint32(descriptor->fd_inode.i_size) - descriptor->offset; 

    bytes_available = descriptor->fs->block_size - file_block_offset;
    if (bytes_available > nm_uint32(descriptor->fd_inode.i_size)) {
        bytes_available = nm_uint32(descriptor->fd_inode.i_size);
    }

    /*
    if (remaining > nm_uint32(descriptor->fd_inode.i_size)) {
        nm_uint32(descriptor->fd_inode.i_size);
        }
    */

    printf("total available=%lu, bytes available in this block=%lu\r\n", total_available, bytes_available);

    devices[descriptor->fs->device_number].seek(&devices[descriptor->fs->device_number], (fs_block_id * descriptor->fs->block_size) + file_block_offset);
    devices[descriptor->fs->device_number].read(&devices[descriptor->fs->device_number], buf, bytes_available);
    descriptor->offset += bytes_available;
    total_available = nm_uint32(descriptor->fd_inode.i_size) - descriptor->offset; 
    remaining -= bytes_available;
    printf("read %lu bytes, total_available=%lu, discard=%lu\r\n", bytes_available, total_available, remaining-total_available);

    return bytes_available;
}
