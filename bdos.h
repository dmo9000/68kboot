#include "stddef.h"

#define VERSION_MAJOR       0
#define VERSION_MINOR       0
#define VERSION_REVISION   	9	

typedef struct _bdos_vtable {
    uint32_t magic;
    uint8_t ver_maj;
    uint8_t ver_min;
    uint8_t ver_rev;
    int errno;
    int (*_open)(const char *pathname, int flags);
    off_t (*_lseek)(int fd, off_t offset, int whence);
    ssize_t (*_read)(int fd, void *buf, size_t count);
    ssize_t (*_write)(int fd, void *buf, size_t count);
    int (*_close)(int fildes);
} _bdos_vtable;

int bdos_init();

