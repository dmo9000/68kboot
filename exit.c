#include <unistd.h>
#include "stdio.h"

#define CPM_EXIT        (0xff7ffc)


void do_exit(int d)
{
    unsigned int * p = (unsigned int *)((char *)CPM_EXIT);
    p[0] = d;
    while (1) { }
}

void exit(int d)
{
    //printf("--> exit(%d)\r\n", d);
    puts("\r\n");
    puts("\r\n");
    do_exit(d);
    while (1) { }
}

