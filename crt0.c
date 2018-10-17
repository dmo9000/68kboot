#include "bdos.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"


//#define DEBUG

extern int main(int argc, char *argv[]);

_bdos_vtable *btvt = NULL;

int _start(int argc, char *argv[])  __attribute__((section(".start")));

int _start(int argc, char *argv[])
{

//    btvt = my_btvt;
    btvt = (_bdos_vtable *) 0x400;
#ifdef DEBUG
    printf("*** _start() *** btvt->magic = 0x%08lx\r\n", btvt->magic);
#endif /* DEBUG */
    assert(btvt->magic == 0xf0e0f0e0);

    return main(argc, argv);

}
