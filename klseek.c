#define __BDOS__

#include "stdio.h"
#include "errno.h"

off_t klseek(int fd, off_t offset, int whence)
{

    _fd *descriptor;
    /* here we just verify offsets and tweak the pointer around. the real magic happens in read/write */

//    printf("\n+lseek(%d, %u, %d)\r\n", fd, offset, whence);

    descriptor = fcntl_get_descriptor(fd);

		if (whence == SEEK_END) {
				printf("SEEK_END!!\r\n");
				printf("descriptor->inode =%u\n", descriptor->inode);
				descriptor->limit = ext2_inode_size(descriptor->inode);
				printf("descriptor->limit =%u\n", descriptor->limit);
				descriptor->offset = descriptor->limit + offset;
				return (descriptor->offset);
				}

		if (whence == SEEK_CUR) {
				if (offset == 0) {
						return descriptor->offset;
						} else {
						descriptor->offset += offset;
						return descriptor->offset;
						}
				/* not reached */
				}

    if (whence == SEEK_SET) {
        descriptor->offset = offset;
        set_errno(0);
        return descriptor->offset;
    }

    set_errno(EINVAL);
    return -1;
}

