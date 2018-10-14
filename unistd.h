#include "stddef.h"

void do_exit(int d);
ssize_t read(int fd, void *buf, size_t count);
int close(int filedes);
#define exit(expr)  do_exit(expr)

