#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "assert.h"
#include "string.h"

/* 32K = 256 sectors */

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
				putchar('.');	
				}
			}
	putchar('\r');
	putchar('\n');
	run_vector = 0x0500;
	run_vector(0, NULL);
}
