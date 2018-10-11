#include "stdio.h"
#include "string.h"
#include "devices.h"
#include "types.h"

_device devices[MAX_DEVICES];
int device_free = 0;

//int dev_register(char *name, int type, int maj, int min, unsigned long addr, unsigned long size, int (*seek), int (*read), int (*write))
//int dev_register(char *name, int type, int maj, int min, unsigned long addr, unsigned long size, int (*seek)(struct _device *, uint32_t), int (*read), int (*write))
int dev_register(char *name, int type, int maj, int min, unsigned long addr, unsigned long size,
                 int (*seek)(struct _device *, uint32_t),
                 int (*read)(struct _device *, unsigned char *, long unsigned int),
                 int (*write))
{
    //printf("dev_register(%s, %u, %u, %u)\n", name, type, maj, min);

    memcpy(&devices[device_free].name, name, strlen(name));
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

int dev_list()
{
    int i = 0;
    printf("id\tname\ttype\tmaj\tmin\tseek    \tread    \twrite    \toffset\r\n");
    puts("\r\n");

    for (i = 0; i< device_free; i++)
    {
        printf("%02u\t%-4s\t%04u\t%03u\t%03u\t0x%08lx\t0x%08lx\t0x%08lx\t0x%08lx\r\n",
               i, devices[i].name, devices[i].type,
               devices[i].maj, devices[i].min, (uint32_t) devices[i].seek,
               (uint32_t) devices[i].read, (uint32_t) devices[i].write, devices[i].offset);
        puts("\r\n");
    }

    return 0;
}

