#include "kstring.h"

size_t  kstrlen(const char *t)
{
    size_t ct = 0;
    while (*t++)
        ct++;
    return ct;
}
