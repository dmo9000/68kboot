#include "bdos.h"
#include "stdio.h"
#include "string.h"

extern int main(int argc, char *argv[]);

_bdos_vtable *btvt = NULL;

int _start(_bdos_vtable *my_btvt, int argc, char *argv[])  __attribute__((section(".start")));

int _start(_bdos_vtable *my_btvt, int argc, char *argv[])  
{

    printf("*** _start() ***\r\n");
    btvt = my_btvt;
    puts("\r\n");
    puts("\r\n");
    return main(argc, argv);

}
