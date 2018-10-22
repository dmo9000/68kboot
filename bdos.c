#include "stdio.h"
#include "bdos.h"
#include "fcntl.h"
#include "unistd.h"

_bdos_vtable bdvt       __attribute__((section(".bdos_vtable")));

int bdos_init()
{
    //printf("bdos_init()\r\n");
    bdvt.magic = 0xF0E0F0E0;
    bdvt.ver_maj = VERSION_MAJOR;
    bdvt.ver_min = VERSION_MINOR;
    bdvt.ver_rev = VERSION_REVISION;
    bdvt._open = open;
    bdvt._read = read;
    bdvt._close = close;
    printf("\r\n68000 BDOS %u.%u.%u\r\n(c) Copyright 2018 - all rights reserved.\r\n\r\n", bdvt.ver_maj, bdvt.ver_min, bdvt.ver_rev);
    //puts("\r\n");
    return 1;
}
