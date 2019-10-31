#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <stdbool.h>
#include <assert.h>
#include "dump.h"

char DFS = '|';
extern bool continue_or_exit ();

int ptr_dump(uint32_t offset, uint32_t limit)
{
    char ptr_addr[32];
    //printf("ptr_dump(0x%08lx)\n\r", offset);
    snprintf((char *) &ptr_addr, 31, "0x%08lx", offset);
//    kernel_printf("address = %s\n\r", ptr_addr);
    realdump(ptr_addr, limit);
    return 0;

}

int dump(char *payload)
{
    return realdump(payload, 0);
}

int realdump(char *payload, uint32_t limit)
{
    uint32_t addr = 0;
    int i = 0;
    int lines = 0;
    unsigned char *ptr = NULL;
    unsigned limit_counter = 0;
    //printf("dump(%s)\n\r", payload);


    if (kernel_strncmp(payload, "0x", 2) == 0) {
        addr = kernel_strtoul(payload, NULL, 16);
    } else {
        addr = kernel_strtoul(payload, NULL, 10);
    }
    if (addr > 0xffffffff) {
        kernel_printf("dump: address out of range\n\r");
        return -1;
    }

    while (lines < 16 && addr <= 0xffffffff && (limit && limit_counter < limit)) {
        kernel_printf("0x%08lx %c ", addr, DFS);
        for (i = 0 ; i < 16; i++) {
            ptr = (unsigned char *) addr + i;
            kernel_printf("%02x ", ptr[0]);
            ptr++;
        }
        ptr -= 16;
        kernel_printf("%c", DFS);

        for (i = 0 ; i < 16; i++) {
            ptr = (unsigned char *) addr + i;
            if (ptr[0] >= 32 && ptr[0] <= 127) {
                //printf("%c", ptr[0]);
                kernel_putchar(ptr[0]);
            } else {
                kernel_printf(".");
            }
            ptr++;
        }
        /* line warp */

        kernel_puts("\n\r");

        addr += 16;
        lines++;

        if (limit) {
            limit_counter += 16;
        }

        if (lines >= 16 && (limit && limit_counter < limit)) {
            if (continue_or_exit()) {
                lines = 0;
                kernel_printf("\n\r");
            }
        }
    }


    kernel_printf("\n\r");

    return 0;
}


extern bool continue_or_exit ()
{

    char c = 0;
    kernel_printf("Continue or exit [Y/n]?\n\r");
    kernel_puts("\n\r");

    while (1) {
        c = kernel_getchar();
        switch(c) {
        case 'Y':
        case 'y':
            return true;
        case 'N':
        case 'n':
            return false;
        default:
            break;
        }
    }
}
