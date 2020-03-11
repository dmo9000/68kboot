#define __BDOS__

#include "stdio.h"
#include "stddef.h"
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include "fcntl.h"
#include "assert.h"
#include "errno.h"
#include "byteorder.h"

extern _device devices[MAX_DEVICES];
extern TTY ktermios;

ssize_t kread(int fd, void *buf, size_t count)
{

    uint32_t file_block_id = 0;
    uint32_t file_block_offset = 0;
    uint32_t fs_block_id = 0;

    uint32_t remaining = count;
    //uint32_t bytes_to_read = 0;

    uint32_t total_available = 0;
    uint32_t bytes_available_in_this_block = 0;
    //uint32_t total_size = 0;
    uint32_t still_available = 0;
    uint32_t total_bytes_read = 0;
    void *ptr = NULL;
    char *kbdata = NULL;
    // uint32_t bytes_read = 0;
    _fd *descriptor = fcntl_get_descriptor(fd);

    assert(descriptor);

    //kernel_printf("__read(%d, 0x%08lx, %u@[%lu/%lu])\r\n", fd, buf, count, descriptor->offset, nm_uint32(descriptor->fd_inode.i_size));
    //kernel_printf("_read(%d, 0x%lx, %u)\r\n", fd, buf, count);

    if (descriptor->state == FD_STATE_STDIN && fd == 0) {
        //printf("read(%d, 0x%lx, %u)\r\n", fd, buf, count);
        //printf("keyboard_read(%d/%d)\r\n", total_bytes_read, count);
        kbdata=(char *) buf;
        /* reading from stdin */

        if ((descriptor->tflags & O_NONBLOCK)) {

            char *  p = (char *) 0xff1000;
            /* non-blocking IO on stdin */
            if (p[0] > 0) {
                /* byte waiting */
                //char * r= (char *) 0xff1002;
                // kbdata[0] = r[0];
                kbdata[0] =kernel_getchar();
                //printf("\t\t\t\t\tkbdata[0] = 0x%02x ['%c']\n",
                //		kbdata[0],
                //			kbdata[0]);
                kbdata[1] = '\0';
                total_bytes_read = 1;
                //printf("returning %d\n", total_bytes_read);
                return total_bytes_read;
            } else {
                /* no data waiting */
                return -1;
            }
        }
        /* "cooked" mode */

        //printf("/* cooked */\n");

        while (total_bytes_read < count) {
            kbdata[total_bytes_read] =kernel_getchar();
            /* ECHO IN COOKED MODE */

            if(ktermios.c_lflag & ECHO) {
                if (kbdata[total_bytes_read] != 0x08) {
                    kernel_putchar(kbdata[total_bytes_read]);
                }
            }

            //printf("byte -> %c\r\n", kbdata[total_bytes_read]);
            /* exit early on RETURN, regardless of how much is read */
            if (kbdata[total_bytes_read] == '\r') {
                total_bytes_read++;
                return total_bytes_read;
            }
            total_bytes_read++;
        }
        return total_bytes_read;
    }


    /* filesystem */

    if (descriptor->offset >= ((EXT2_NDIR_BLOCKS * descriptor->fs->block_size) + 256 * (descriptor->fs->block_size) +
                               ((256*256) * (descriptor->fs->block_size)))
       ) {
        kernel_printf("read() - offset %lu is out of range\r\n", descriptor->offset);
        // EOF - return 0 */
        //return 0;
        set_errno(EIO);
        return -1;
    }

    if (descriptor->offset >= nm_uint32(descriptor->fd_inode.i_size)) {
        // EOF
        return 0;
    }

    total_available = nm_uint32(descriptor->fd_inode.i_size) - descriptor->offset;
    still_available = ( remaining < total_available ? remaining : total_available );

    ptr = buf;

    while (remaining > 0 && still_available > 0) {

        file_block_id = descriptor->offset / descriptor->fs->block_size;
        file_block_offset = descriptor->offset % descriptor->fs->block_size;
        //printf("** next block is within range -> block %lu, offset %lu\r\n", file_block_id, file_block_offset);

        fs_block_id = ext2_get_inode_block(&descriptor->fd_inode, file_block_id);
        //printf("++ target offset = (0x%08lx+0x%08lx:0x%08lx+0x%08lx)\r\n", fs_block_id, file_block_offset, (
        //          fs_block_id * descriptor->fs->block_size), file_block_offset);

        //assert(ext2_block_read(descriptor->fs, 0xE000, fs_block_id));

        bytes_available_in_this_block = descriptor->fs->block_size - file_block_offset;
        /*
        if (bytes_available_in_this_block > nm_uint32(descriptor->fd_inode.i_size)) {
            bytes_available_in_this_block = nm_uint32(descriptor->fd_inode.i_size);
        }
        */
        if (bytes_available_in_this_block > still_available) {
            bytes_available_in_this_block = still_available;
        }

        //printf("++ total available=%lu, bytes available in this block=%lu, read_remaining=%lu\r\n", total_available, bytes_available_in_this_block, remaining);

        devices[descriptor->fs->device_number].seek(&devices[descriptor->fs->device_number], (fs_block_id * descriptor->fs->block_size) + file_block_offset);
        devices[descriptor->fs->device_number].read(&devices[descriptor->fs->device_number], ptr, bytes_available_in_this_block);
        descriptor->offset += bytes_available_in_this_block;
        total_available = nm_uint32(descriptor->fd_inode.i_size) - descriptor->offset;
        remaining -= bytes_available_in_this_block;
        ptr += bytes_available_in_this_block;
        total_bytes_read += bytes_available_in_this_block;
        still_available = ( remaining < total_available ? remaining : total_available );
        //printf("++ read %lu bytes, total_available=%lu, read_remaining=%lu, still_available=%lu\r\n", bytes_available_in_this_block, total_available, remaining, still_available);
    }

    set_errno(0);
    return total_bytes_read;
}
