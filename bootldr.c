#include "stdio.h"
#include "unistd.h"
#include "types.h"

/* 32K = 256 sectors */

int main(int argc, char *argv[])
{
  off_t dma_addr = 0x8400;

	//printf("Initial DMA address is 0x%lx ...\r\n", dma_addr);
	puts("\r\n");
	ptr_dump(0x8400);

	exit(1);
}
