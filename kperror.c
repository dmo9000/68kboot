#define __BDOS__

#include "stdio.h"
#include <string.h>
#include "errno.h"

int errno;

void perror(const char *s)
{
    kernel_printf("%s: %s (%u)\r\n", (const char *) s, (const char *) kernel_strerror(errno), errno);

}

