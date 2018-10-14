#include "stdio.h"

int puts(const char *s)
{
    while (s[0] != '\0') {
        putchar(s[0]);
        s++;
    }
    return 0;
}

