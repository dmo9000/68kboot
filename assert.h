#include "stdlib.h"
#include <unistd.h>

void _ASSERT(char *error, char *file, int line);
#define assert(expr) \
    if (!(expr)) \
        _ASSERT(#expr, __FILE__, __LINE__)

