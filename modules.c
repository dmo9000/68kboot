#include <stdio.h>
#include "modules.h"

#define DFS '|'
#define NULL 0

int modules(char *payload)
{
    int jumpIndex = 0;
    jmpTable *cmdptr = NULL;

    kprintf("\n\r");
    cmdptr = (jmpTable*) &jmptbl[jumpIndex];
    kprintf("         COMMAND  %c  ADDRESS\n\r", DFS);
    while (cmdptr->cmdptr) {
        kprintf("%16s  %c  0x%04lx\n\r", cmdptr->command, DFS, (unsigned long) cmdptr->cmdptr);
        jumpIndex++;
        cmdptr = (jmpTable *) &jmptbl[jumpIndex];
    }
    kernel_puts("\n\r");

    payload = NULL;
    return 0;
}

