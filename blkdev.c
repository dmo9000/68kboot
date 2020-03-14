#include "kstdlib.h"
#include "stdio.h"
#include "blkdev.h"
#include "devices.h"

extern _device devices[MAX_DEVICES];


int blkdevseek(uint8_t dev, off_t offset)
{
//		kprintf("_blkdevseek(%u, %lu)\n\r", dev, offset); 
    devices[dev].seek(&devices[dev], offset);
		return 0;
}
int blkdevread(uint8_t dev, char *buffer, size_t len) 
{
//		kprintf("_blkdevread(%u, ..., %lu)\n\r", dev, len); 
    devices[dev].read(&devices[dev], (unsigned char *) buffer, 
					len);
		return 0;
}
int blkdevwrite(uint8_t dev, char *buffer, size_t len) 
{
		kprintf("_blkdevwrite(%u, ..., %lu)\n\r", dev, len); 
		return 0;

}


