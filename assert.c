#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>

void _ASSERT(char *error, char *file, int line)
{
    puts("\r\n");
    printf("+++ assert '%s' at %s, line %d\r\n", error, file, line);
    exit(1);
}

