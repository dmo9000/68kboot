#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "bdos.h"
#include "assert.h"

extern _bdos_vtable *btvt;

#define exit(f) return f;

int main(int argc, char *argv[])
{

    char buffer[4096];
    int fd = 0;
    int i = 0;
    int rd = 0;

    // printf("Hello world! vtdb.magic = 0x%08lx\r\n", btvt->magic);

    /*
        printf("received %d arguments\r\n", argc);
        for (i = 0; i < argc; i++) {
            printf("%d: %s\r\n", i, argv[i]);
            }
        puts("\r\n");
    */

    fd = btvt->_open("/hello1.txt", O_RDONLY);

//    printf("fd = %d\r\n", fd);

    if (fd != -1) {
        rd = btvt->_read(fd, &buffer, 4096);
//       printf("rd = %d\r\n", rd);
        for (i = 0; i < rd; i++) {
            putchar(buffer[i]);
        }
        btvt->_close(fd);
    }

    puts("\r\n");
//    return 0;
    exit(0);

}




