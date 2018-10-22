#include "stdint.h"
#include "string.h"
#include "sbrk.h"
#include "assert.h"

uint32_t  heap_marker  __attribute__((section(".heap_marker")));
unsigned char *heap_ptr = NULL;
unsigned char *heap_next = NULL;
extern void *_end;

void init_heap()
{
    printf("init_heap()\r\n");
    puts("\r\n");
    heap_ptr = &_end;
    heap_next = heap_ptr;

}

void *get_heap_marker()
{
    return (void*) &heap_marker;
}

void *sbrk(intptr_t increment)
{
    void *heap_return = NULL;
    //printf("sbrk(0x%08lx)\r\n", increment);

    assert(increment >= 0) ;

    if (increment == 0) {
        //printf("-> sbrk(0x%08lx)\r\n", increment);
        return heap_next;
    }
    //printf("*> sbrk(0x%08lx)\r\n", increment);
    heap_return = heap_next;

    if (heap_return + (uint32_t) increment >= 0x1000000) {
        return (void *) -1;
    }

    if (heap_return >= 0x1000000) {
        /* out of memory */
        //printf("sbrk(): out of memory\r\n");
        //assert(NULL);
        return (void *) -1;
    }

    heap_next += (uint32_t) increment;
    return heap_return;
}

