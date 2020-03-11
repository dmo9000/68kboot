#define __BDOS__

#include "stdio.h"
#include "stddef.h"
#include <string.h>
#include <sys/types.h>
#include "fcntl.h"
#include "assert.h"
#include "errno.h"
#include "byteorder.h"


ssize_t kwrite(int fd, void *buf, size_t count)
{
    int i = 0;
    char *p = (char *) buf;
    //printf("kwrite(%d, ..., %u)\n\r", fd, count);
    /* write to stdout/stderr */
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        if (count && buf) {
            for (i = 0; i < count; i++) {
                kernel_putchar((char) p[i]);
            }
        }
        return count;
    }
    set_errno(EPERM);
    return -1;
}
