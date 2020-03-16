#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#include "assert.h"
#include <string.h>
#include "disk.h"

/* 32K = 256 sectors */

char *spinner = "|/-\\|/-\\";

void boot_spinner()
{
    for (int i = 0; i < 8; i++) {
        boot_putchar(spinner[i]);
        boot_putchar(0x08);
        for (int j = 0; j < 100000000; j++) {
            /* delay */
        }
    }
    boot_putchar('.');
    return;
}

int boot_putchar(int c)
{
    char * p = (char *)0xff1002;
    p[0] = c;
    return 0;
}

int kernel_puts(const char *s)
{
    while (s[0] != '\0') {
        boot_putchar(s[0]);
        s++;
    }
    return 0;
}


int main(int argc, char *argv[])
{
    int start_sector = 258;
    int end_sector = 1024;		/* 64K - need to increase this if main.out gets too large */
    int current_sector = start_sector;
    int (*run_vector)(int argc, char *argv[]);
    int sector_count = 0;

    off_t dma_addr = 0x500;

    disk_set_drive(2);
    while (current_sector < end_sector) {
        disk_set_dma(dma_addr);
        disk_read_sector(current_sector);
        current_sector++;
        dma_addr += 0x80;
        sector_count ++;
        if (!(sector_count % 8)) {
            boot_putchar('.');
//							boot_spinner();
        }
    }
    boot_putchar('\r');
    boot_putchar('\n');
    run_vector = (int (*)(int,  char **)) 0x0500;
    run_vector(0, NULL);
}
