#include "bdos.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "sbrk.h"

extern void *_end;

#define DEBUG

extern int main(int argc, char *argv[]);

_bdos_vtable *btvt = NULL;

int _start(int argc, char *argv[])  __attribute__((section(".start")));

int _start(int argc, char *argv[])
{
    void *heap_marker = NULL;
    void *heap_marker2 = NULL; 
    btvt = (_bdos_vtable *) 0x400;
    heap_marker = get_heap_marker();
    heap_marker2 = _end;
    //memset(heap_marker, 0xFE, 128);
#ifdef DEBUG
    printf("*** _start() *** btvt->magic = 0x%08lx, heap_marker = 0x%08lx, _end = 0x%08lx\r\n", btvt->magic, heap_marker, &_end);
#endif /* DEBUG */
    assert(btvt->magic == 0xf0e0f0e0);
    init_heap(); 

    return main(argc, argv);

}
