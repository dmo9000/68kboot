#include "stdio.h"
#include "bdos.h"
#include "fcntl.h"
#include "unistd.h"

_bdos_vtable bdvt       __attribute__((section(".bdos_vtable")));

int bdos_init()
{
    //printf("bdos_init()\r\n");
    bdvt.magic = 0xF0E0F0E0;
    bdvt._open = open;
    bdvt._read = read;
    bdvt._close = close;
    //puts("\r\n");
    return 1;
}
