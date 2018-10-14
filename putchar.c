#include "stdio.h"

int putchar(int c)
{
    char * p = (char *)0xff1002;
    p[0] = c;
    return 0;
}

