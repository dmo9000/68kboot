#include <sys/types.h>
#include "disk.h"

#define DISK_BASE       0xff0000L
#define DISK_SET_DMA    (DISK_BASE)
#define DISK_SET_DRIVE  (DISK_BASE+4)
#define DISK_SET_SECTOR (DISK_BASE+8)
#define DISK_READ       (DISK_BASE+12)
#define DISK_WRITE      (DISK_BASE+16)
#define DISK_STATUS     (DISK_BASE+20)
#define DISK_FLUSH      (DISK_BASE+24)



int disk_set_drive(int disk)
{
//    kernel_printf("disk_set_drive(%d)\r\n", disk);
    uint32_t * p = (uint32_t *)((char *)DISK_SET_DRIVE);
    p[0] = disk;
    return 0;

}

int disk_set_dma(uint32_t addr)
{
//    kernel_printf("disk_set_dma(0x%08x)\r\n", addr);
    uint32_t *p = (uint32_t *)((char *)DISK_SET_DMA);
// LITTLE ENDIAN ONLY;
//    p = htonl(addr);
    p[0] = addr;
    return 0;
}

int disk_read_sector(uint32_t sector)
{
    //  kernel_printf("disk_read_sector([%d] 0x%08x)\r\n", sector, sector);
    uint32_t *p = (uint32_t *)((char *)DISK_READ);
    p[0] = sector;
    return 0;
}
