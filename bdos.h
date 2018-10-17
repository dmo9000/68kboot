#include "stddef.h"


typedef struct _bdos_vtable {
                            uint32_t bdos_vtable_magic;
                            int (*_open)(const char *pathname, int flags);
                            off_t (*_lseek)(int fd, off_t offset, int whence);
                            ssize_t (*_read)(int fd, void *buf, size_t count);
                            ssize_t (*_write)(int fd, void *buf, size_t count);
                            int (*_close)(int fildes);
                            } _bdos_vtable;

int bdos_init();

