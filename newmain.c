#include "stdio.h"
#include "stdlib.h"
#include "string.h"
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
    unsigned char *somemem = NULL;
    uint32_t mallsize = 1;

    // printf("Hello world! vtdb.magic = 0x%08lx\r\n", btvt->magic);

        printf("received %d arguments\r\n", argc);
        for (i = 0; i < argc; i++) {
            printf(" -> %d: %s\r\n", i, argv[i]);
            }
        puts("\r\n");

    fd = btvt->_open("/hello1.txt", O_RDONLY);

//    printf("fd = %d\r\n", fd);

    /*
        if (fd != -1) {
            rd = btvt->_read(fd, &buffer, 4096);
            for (i = 0; i < rd; i++) {
                putchar(buffer[i]);
            }
            btvt->_close(fd);
       }
    */

//    printf("sbrk(0) = 0x%08lx\r\n", sbrk(0));
    printf("malloc(%lu) = ", mallsize);
    somemem = my_malloc(mallsize);
    while (somemem != NULL && mallsize < 0xFFFFFFFF && mallsize > 0) {
        printf("0x%08lx\r\n", somemem);
        my_free(somemem);
        mallsize = mallsize << 1;
        printf("malloc(%lu) = ", mallsize);
        somemem = my_malloc(mallsize);
    }

    printf("... FAILED\r\n");
//    printf("sbrk(0) = 0x%08lx\r\n", sbrk(0));
//    printf("freeing ...\r\n");
//    free(somemem);
//    printf("sbrk(0) = 0x%08lx\r\n", sbrk(0));

    puts("\r\n");
//    return 0;
    exit(0);

}




