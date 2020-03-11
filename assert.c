#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>

void _ASSERT(char *error, char *file, int line)
{
    kernel_puts("\r\n");
    kprintf("+++ assert '%s' at %s, line %d\r\n", error, file, line);
    while (1) {}
}

