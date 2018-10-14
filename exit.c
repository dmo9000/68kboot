#include "unistd.h"

#define CPM_EXIT        (0xff7ffc)


void do_exit(int d)
{
    unsigned int * p = (unsigned int *)((char *)CPM_EXIT);
    p[0] = d;
    while (1) { }
}

