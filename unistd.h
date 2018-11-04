#include "stddef.h"

void do_exit(int d);
void exit(int d);
void _exit(int d);
ssize_t read(int fd, void *buf, size_t count);
int close(int filedes);
//#define exit(expr)  do_exit(expr)
off_t lseek(int fd, off_t offset, int whence);
