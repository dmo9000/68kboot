#ifndef __DEVICES_H__
#define __DEVICES_H__

#define MAX_DEVICES		64

/* devices types */

#define DEVTYPE_CHAR	0
#define DEVTYPE_BLOCK	1

/* Major numbers */

#define	DEV_RAMDEV		0
#define	DEV_FLOPPY		1
#define DEV_IDE			2
#define DEV_CPMIO       0xFF

/* this crappy interface needs a rewrite */

typedef struct _device {
    unsigned char name[16];
    unsigned char type;
    unsigned int maj;
    unsigned int min;
    unsigned long offset;
    unsigned long addr;
    unsigned long size;
//						int (*seek)();
    int (*seek)(struct _device *, uint32_t);
//						int (*read)();
    int (*read)(struct _device *, unsigned char *, long unsigned int);
    int (*write)();

} _device;

int dev_list();
int dev_register(char *name, int type, int maj, int min, unsigned long addr, unsigned long size,
                 int (*seek)(struct _device *, uint32_t),
                 int (*read)(struct _device *, unsigned char *, long unsigned int),
                 int (*write));


#endif /* __DEVICES_H__ */
