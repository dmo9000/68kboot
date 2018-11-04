#include "bdos.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "sbrk.h"

extern void *_end;

//#define DEBUG

extern int errno;
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
    if (btvt->ver_rev < 3) {
        printf("This program requires BDOS v0.0.3 (minimum) to run.\r\n");
        puts("\r\n");
        return 0;
    }

    init_heap();
    return main(argc, argv);

}

int _bdos_geterrno()
{
//    errno = btvt->errno;
    return btvt->errno;
}

int _bdos_seterrno(int e)
{
    btvt->errno = e;
//    errno = e;
    return btvt->errno;
}
