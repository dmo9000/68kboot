#define __BDOS__

#include "stdio.h"
#include "errno.h"

off_t lseek(int fd, off_t offset, int whence)
{

    _fd *descriptor;
    /* here we just verify offsets and tweak the pointer around. the real magic happens in read/write */

    //printf("\n+lseek(%d, %u, %d)\r\n", fd, offset, whence);

    descriptor = fcntl_get_descriptor(fd);

    if (whence == SEEK_SET) {
        descriptor->offset = offset;
        set_errno(0);
        return descriptor->offset;
    }

    set_errno(EINVAL);
    return -1;
}

