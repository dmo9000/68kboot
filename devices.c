#include "stdio.h"
#include <string.h>
#include "devices.h"
#include <sys/types.h>
#include "kernel.h"
#include "stat.h"

_device devices[MAX_DEVICES];
int device_free = 0;

const char *device_type(int type)
{
	switch (type) {
								case DEVTYPE_CHAR:
									return (const char *) "CHR";
								case DEVTYPE_BLOCK:
									return (const char *) "BLK";
								default:
									return (const char *) "UNKNOWN";
								break;
								};

}

/*
int dev_register(char *name, int type, int maj, int min, unsigned long addr, unsigned long size,
                 int (*seek)(struct _device *, uint32_t),
                 int (*read)(struct _device *, unsigned char *, long unsigned int),
                 int (*write)(struct _device *, unsigned char *, long unsigned int))
*/
int dev_register(char *name, int type, int maj, int min, unsigned long addr, unsigned long size,
                 int (*seek)(struct _device *, uint32_t),
                 int (*read)(struct _device *, unsigned char *, long unsigned int),
                 int (*write)(struct _device *, unsigned char *, long unsigned int))
{
    //printf("dev_register(%s, %u, %u, %u)\n", name, type, maj, min);

    kernel_memcpy(&devices[device_free].name, name, strlen(name));
    devices[device_free].type = type;
    devices[device_free].min = min;
    devices[device_free].maj = maj;
    devices[device_free].seek = (int (*)()) seek;
    devices[device_free].read = (int (*)()) read;
    devices[device_free].write = (int (*)()) write;
    devices[device_free].offset = 0;
    devices[device_free].addr = addr;
    devices[device_free].size = size;
    device_free++;

    return(0);
}

int dev_getdisknumber(int id)
{
    return devices[id].min;
}

int dev_list()
{

		int i = 0;
		kernel_printf("\n\r");
    kernel_printf("ID\tNAME\tTYPE\tMAJ:MIN\tSEEK    \tREAD    \tWRITE    \tOFFSET\n\r");

    for (i = 0; i< device_free; i++)
    {
        kernel_printf("%02u\t%-4s\t%-4s\t%03u:%03u\t%08lx\t%08lx\t%08lx\t%08lx\n\r",
               i, 
							 devices[i].name, 
							 device_type(devices[i].type),
               devices[i].maj, devices[i].min, (uint32_t) devices[i].seek,
               (uint32_t) devices[i].read, (uint32_t) devices[i].write, devices[i].offset);
    }
		kernel_printf("\n\r");
    return 0;
}

