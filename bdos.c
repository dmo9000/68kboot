#define __BDOS__

#include "stdio.h"
#include "bdos.h"
#include "fcntl.h"
#include "unistd.h"
#include "errno.h"
#include "string.h"

_bdos_vtable bdvt       __attribute__((section(".bdos_vtable")));
extern int main(int argc, char *argv[]);

int bdos_init()
{
    //printf("bdos_init()\r\n");
    memset((void *) 0x0400, 0, 256);
    bdvt.magic = 0xF0E0F0E0;
    bdvt.ver_maj = VERSION_MAJOR;
    bdvt.ver_min = VERSION_MINOR;
    bdvt.ver_rev = VERSION_REVISION;
    bdvt._open = open;
    bdvt._read = read;
    bdvt._close = close;
    printf("\r\n68000 BDOS %u.%u.%u\r\n(c) Copyright 2018 - all rights reserved.\r\n\r\n", bdvt.ver_maj, bdvt.ver_min, bdvt.ver_rev);
    //printf("main = 0x%lx\r\n", main);
    //puts("\r\n");
    return 1;
}

int set_errno(int d)
{
    errno = d;
    bdvt.errno = d;
    return bdvt.errno;
}
