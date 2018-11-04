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

    printf("received %d arguments\r\n", argc);
    for (i = 0; i < argc; i++) {
        printf(" -> %d: %s\r\n", i, argv[i]);
    }
    puts("\r\n");

    somemem = malloc(mallsize);
    while (somemem != NULL && mallsize < 0xFFFFFFFF && mallsize > 0) {
        //   printf("0x%08lx\r\n", somemem);
        free(somemem);
        mallsize = mallsize << 1;
        // printf("malloc(%lu) = ", mallsize);
        somemem = malloc(mallsize);
    }

    printf("Largest successfull malloc(%u)\r\n", mallsize >> 1);
    puts("\r\n");
    exit(0);

}




