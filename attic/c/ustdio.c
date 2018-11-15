#include "bdos.h"

extern _bdos_vtable *btvt;

int open(const char *pathname, int flags)
{

    return btvt->_open(pathname, flags);
}


ssize_t read(int fd, void *buf, size_t count)
{

    return btvt->_read(fd, buf, count);

}

int close(int fd)
{

    return btvt->_close(fd);
}

