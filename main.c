#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "modules.h"
#include "dump.h"
#include "devices.h"
#include "assert.h"
#include "byteorder.h"
#include "disk.h"

void do_exit(int d);

extern ext2_fs ext2_rootfs;

#define CPM_EXIT        (0xff7ffc)
#define SECTOR_SIZE    128
#define exit(expr)  do_exit(expr)

int select_disk(char *s);
int get_inode(char *s);
int ls(char *s);
int cd(char *s);
int cat(char *s);

const jmpTable jmptbl[] = {
    {select_disk, "disk"},
    {dev_list, "devices"},
    {dump, "dump"},
    {modules, "?"},
    {get_inode, "inode"},
    {ls, "ls"},
    {cd, "cd"},
    {cat, "cat"},
    {0x0, ""}
};

bool errorFlag = false;
char *x = NULL;            /* ugly global pointer */
bool command_was_executed = false;


typedef uint32_t mathRegister;
mathRegister parseToken ();
mathRegister parseCommand ();
mathRegister parseSum ();
mathRegister parseProduct ();
mathRegister parseFactor ();
bool isTerminator (char c);
bool isWhitespace (char c);

#define MAX_STRING  80

char parseString[MAX_STRING];


int putchar(int c);
int puts(const char *s);

typedef unsigned long size_t;
size_t strlen(const char *t);

int cpmsim_seek(struct _device *, uint32_t);
int cpmsim_read(struct _device *d, unsigned char *buf, unsigned long size);

void _ASSERT(char *error, char *file, int line)
{
    printf("+++ assert '%s' at %s, line %d\r\n", error, file, line);
    exit(1);
}

void do_exit(int d)
{
    unsigned int * p = (unsigned int *)((char *)CPM_EXIT);
    p[0] = d;
    while (1) { }
}

int main()
{
    int length = 0;
    int result = 0;
    unsigned char c = 0;
    static char command[2048];

    dev_register("E:", DEVTYPE_BLOCK, DEV_CPMIO, 4, 0x0, 0x0, cpmsim_seek, cpmsim_read, 0x0);

    while (1) {
        //printf("%s%u/boot> ", (is_big_endian() ? "be" : "le"), sizeof(unsigned int)*8);
        printf("boot> ");
        memset(&command, 0, 2048);
        c = getchar();
        while (c != '\r') {
            printf("%c", c);
            command[length] = c;
            length++;
            c = getchar();
        }
        puts("\r\n");
        if (length == 0) {
            return 0;
        }

        x = (char *) &command;
        result = parseSum ();
        if (!errorFlag && !command_was_executed)
        {
            printf ("result = %u\r\n", result);
        }
        errorFlag = false;
        length = 0;
    }

    puts("\r\n");
    return 0;
}

int getchar()
{
    char *  p = (char *) 0xff1002;
    return p[0];
}

int putchar(int c)
{
    char * p = (char *)0xff1002;
    p[0] = c;
    return 0;
}

int puts(const char *s)
{
    while (s[0] != '\0') {
        putchar(s[0]);
        s++;
    }
    return 0;
}

size_t strlen(const char *t)
{
    size_t ct = 0;
    while (*t++)
        ct++;
    return ct;
}

int get_inode(char *s)
{
    ext2_inode my_inode;
    uint32_t inode_number = 0;
    inode_number = strtoul(s, NULL, 10);
//    printf("get_inode(%s)[%lu]\r\n", s, inode_number);
    assert(ext2_inode_lookup(inode_number, &my_inode, true));
    return 1;
}


int select_disk(char *s)
{
    int disk_number = 0;
    //printf("selecting disk: %s\r\n", s);
    disk_number = strtoul(s, NULL, 10);
    if (!(disk_number >=0 && disk_number <= 0x0F)) {
        printf("invalid disk number %u\r\n", disk_number);
        return 0;
    }
    disk_set_drive(disk_number);
    //disk_set_dma(0x8000);
    /* should be e2fs superblock @ byte 0x400 (1024) */
    //disk_read_sector(8);
    //dump("0x08000");
    ext2_probe();
    //puts("\r\n");
    return 0;
}


mathRegister
parseSum ()
{
//   printf("parseSum(%s)\r\n", x);
    mathRegister pro1 = parseProduct ();
    while (*x == '+')
    {
        mathRegister pro2;
        ++x;
        pro2 = parseProduct ();
        pro1 = pro1 + pro2;
    }
//    printf("returning = %u\r\n", pro1);
    return pro1;

}

mathRegister
parseProduct ()
{

//    printf("parseProduct(%s)\r\n", x);
    mathRegister fac1 = parseFactor ();
    while (*x == '*')
    {
        mathRegister fac2;
        ++x;
        fac2 = parseFactor ();
        fac1 = fac1 * fac2;
    }
    return fac1;

}


mathRegister
parseFactor ()
{

    // printf("parseFactor(%s)\r\n", x);
    mathRegister sum1 = 0;
    //char *ptr = NULL;
    int jmpIndex = 0;
    int (*jumpPtr) (char *payload);
    if (*x >= '0' && *x <= '9')
    {
        while (*x >= '0' && *x <= '9')
        {
            sum1 = (sum1 * 10) + (*x - '0');
            *x++;
        }
        return sum1;
    }
    else if (*x == '(')
    {
        mathRegister sum;
        ++x;            /* consume ( */
        sum = parseSum ();
        ++x;            /* consume ) */
        return sum;
    }
    else
    {
        parseToken ();
        jumpPtr = jmptbl[jmpIndex].cmdptr;

        if (!strlen (parseString))
        {
            errorFlag = true;
            return 0;
        }

        while (jumpPtr > 0)
        {
            //printf("%u: %s ? %s\n", jmpIndex, jmptbl[jmpIndex].command, parseString);
            if (strncmp
                    (jmptbl[jmpIndex].command, parseString,
                     strlen (jmptbl[jmpIndex].command)) == 0)
            {
                command_was_executed = true;
                return jumpPtr (x);
            }
            jmpIndex++;
            jumpPtr = jmptbl[jmpIndex].cmdptr;
        }

        printf ("syntax error: %s\r\n", parseString);
        errorFlag = true;
        return 0;
    }
}


mathRegister
parseToken ()
{
    int i = 0;
    //char *p = x;
    //  printf("parseToken(%s)\r\n", x);
    memset (&parseString, 0, MAX_STRING);
    while (!isWhitespace (x[i]) && !isTerminator (x[i]) && i < MAX_STRING)
    {
        parseString[i] = x[i];
        i++;
    }
    parseString[i] = '\0';
    x += i;
    while (isWhitespace (x[0]) && !isTerminator (x[0]))
    {
        x++;
    }
    return 0;
}

bool
isTerminator (char c)
{
//   printf(" > isTerminator(%c) = %d\r\n", c, c);
    switch (c)
    {
    case '\0':
        return true;
    default:
        return false;
        break;
    }

}

bool
isWhitespace (char c)
{
//    printf(" > isWhitespace(%c) = %d\r\n", c, c);
    switch (c)
    {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        return true;
    default:
        return false;
        break;
    }

}


int cpmsim_seek(struct _device *d, uint32_t address)
{
    //printf("cpm_seek(d->%s, %lu)\r\n", d->name, address);
    /* not aligned to sector boundary */
    //assert(!(address % SECTOR_SIZE));
    d->offset = address;
    return 0;

}


int cpmsim_read(struct _device *d, unsigned char *buf, uint32_t size)
{
    uint32_t start_sector = 0;
    //uint32_t end_sector = 0;
    uint32_t current_sector = 0;
    uint32_t sector_offset = 0;
    //uint32_t sector_count = 0;
    uint32_t remaining = size;
    //uint32_t byte_read_count = 0;
    uint32_t current_offset = 0;
    uint32_t bytes_available = 0;
    unsigned char *ptr = buf;
    //printf("cpm_read(d->%s, [0x%08lx]->0x%08lx, %lu)\r\n", d->name, d->offset, buf, size);

    start_sector = d->offset / SECTOR_SIZE;
    sector_offset = d->offset % SECTOR_SIZE;
    //sector_count = (size / SECTOR_SIZE) + (size % SECTOR_SIZE ? 1 : 0);
    //end_sector = start_sector + sector_count;
    //printf("->(sector=%u:offset=%u:sector_count=%u:end_sector=%u)\r\n", start_sector, sector_offset, sector_count, end_sector);
    // assert(!(d->offset % SECTOR_SIZE));
    disk_set_dma(0xF000);
    /* can't read partial head block right now */
    //assert(!sector_offset);

    current_sector = start_sector;
    current_offset = sector_offset;



    while (remaining > 0) {
        assert(remaining > current_offset);
        assert(current_offset <= SECTOR_SIZE);
        bytes_available = SECTOR_SIZE - current_offset;
        if (bytes_available > remaining) {
            bytes_available = remaining;
            }
        disk_read_sector(current_sector);
        memcpy(ptr, (const void *) 0xF000 + current_offset, bytes_available);
        remaining -= bytes_available;
        ptr+= bytes_available;
        current_offset = 0;
        current_sector++;
    }
    return 0;
}

bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

uint16_t nm_uint16(uint16_t val)
{
    return (is_big_endian() ? swap_uint16(val) : val);

}

uint32_t nm_uint32(uint32_t val)
{
    return (is_big_endian() ? swap_uint32(val) : val);

}

int ls(char *s)
{
    //printf("ls(%s)\r\n", s);
    if (!ext2_rootfs.active) {
        printf("error: no active filesystem\r\n");
        puts("\r\n");
        return 0;
    }
    assert(ext2_rootfs.active);
    assert(ext2_rootfs.cwd_inode);
    assert(ext2_list_directory(ext2_rootfs.cwd_inode));
    return 0;
}

int cd(char *s)
{
    uint32_t target_inode = 0;
    //printf("cd(%s)\r\n", s);
    if (!ext2_rootfs.active) {
        printf("error: no active filesystem\r\n");
        puts("\r\n");
        return 0;
    }

    assert(ext2_rootfs.active);
    assert(ext2_rootfs.cwd_inode);
    target_inode = ext2_path_to_inode(s);
    //printf("cd: target inode = %u\r\n", target_inode);

    if (!target_inode) {
        printf("%s: directory does not exist\r\n", s);
        return 0;
        }

    switch(isdirectory(target_inode)) {
    case true:
        printf("[changed directory to %s]\r\n", s);
        ext2_rootfs.cwd_inode = target_inode;
        break;
    case false:
        printf("%s: not a directory\r\n", s);
        return 0;
        break;
    default:
        printf("huh?\r\n");
        break;
    }

    return 0;
}

int cat(char *s)
{

    int cat_fd = 0;
    cat_fd = open(s, O_RDONLY);

    if (cat_fd == -1) {
        perror("open");
        return 0;
        }
    return 0;
}
