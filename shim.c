#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "kernel.h"
#include "fcntl.h"
#include "modules.h"
#include "dump.h"
#include "devices.h"
#include "assert.h"
#include "byteorder.h"
#include "bdos.h"
#include "environ.h"
#include "disk.h"
#include "fletcher16.h"
#include "kopen.h"
#include "kread.h"
#include "kclose.h"
#include "kgetenv.h"

int errno;

void do_exit(int d);

extern ext2_fs ext2_rootfs;
extern _bdos_vtable bdvt;

#define SECTOR_SIZE    128
#define exit(expr)  do_exit(expr)

int select_disk(char *s);
int get_inode(char *s);
int ls(char *s);
int cd(char *s);
int cat(char *s);
int load(char *s);
int run(char *s);
int run_f16(char *s);
int run_f16f(char *s);
int quit(char *s);
char *search_path(char *s);
int exec_run(char *pname, char *s);
extern int loadelf(char *s);
extern int bdos_version(char *s);

//#define BDOS_DEBUG_CMDS

const jmpTable jmptbl[] = {
    {select_disk, "disk"},
    {dev_list, "devices"},
    {dump, "dump"},
    {modules, "?"},
#ifdef BDOS_DEBUG_CMDS
    {get_inode, "inode"},
    {ls, "bls"},
    {cd, "bcd"},
    {cat, "bcat"},
    {run, "run"},
    {run_f16, "f16"},
    {run_f16f, "f16f"},
    {loadelf, "loadelf"},
#endif /* BDOS_DEBUG_CMDS */
    {load, "load"},
    {quit, "quit"},
    {quit, "exit"},
    {quit, "q"},
    {bdos_version, "version"},
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

#define MAX_ARGS    	16
#define MAX_STRING  	80

char parseString[MAX_STRING];

#define CHAR_ESCAPE 27

typedef unsigned long size_t;
//size_t kstrlen(const char *t);


#define ETX		0x03			/* end of text */
#define EOT		0x04			/* end of transmission */
#define BS		0x08			/* backspace */

ext2_inode my_inode;

char environment[MAX_ENVIRON];

int kernel_putchar(int c)
{
    char * p = (char *)0xff1002;
    p[0] = c;
    return 0;
}

int kernel_puts(const char *s)
{
    while (s[0] != '\0') {
        kernel_putchar(s[0]);
        s++;
    }
    return 0;
}



void _ASSERT(char *error, char *file, int line)
{
    kernel_puts("\r\n");
    kprintf("+++ assert '%s' at %s, line %d\r\n", error, file, line);
    while (1) { }
}

int main()
{
    asm("movel #1048572,(%sp)");
    bdos_init();
    supermain();
}

int supermain()
{
    int length = 0;
    int result = 0;
    unsigned char c = 0, n = 0;
    static char command[2048];
    char buf[2];

    kernel_memset(&command, 0, 2048);
prompt_start:
    while (1) {
        kprintf("%c[37m""shim> ", 27);
        if (length) {
            /* in case screen was cleared, we don't cancel input */
            kprintf("%s", command);
        }
        //c =kernel_getchar();
        n = bdvt._read(STDIN_FILENO, &buf, 1);
        c = buf[0];
        while (c != '\r') {
            switch (c) {
            case 12:
                /* CTRL+L */
                //kprintf("/* clear screen */\r\n");
                /* DIRTY LOL */
                kprintf ("%c[H%c[2J", CHAR_ESCAPE, CHAR_ESCAPE);
                kprintf ("%c[1;1H", CHAR_ESCAPE);
                goto prompt_start;
                break;
            case 27:
                break;
            case BS:
                if (length) {
                    /* move cursor back, erase, move cursor back */
                    kprintf("\b\ \b");
                    command[length-1] = '\0';
                    length -= 1;
                }
                break;
            case ETX:
                kprintf("^C\r\n");
                kernel_puts("\r\n");
                length = 0;
                goto read_prompt;
                break;
            case EOT:
                //kprintf("<EOT>");
                kprintf("quit\r\n");
                quit(NULL);
                break;
            default:
                command[length] = c;
                length++;
            }
            //c =kernel_getchar();
            n = bdvt._read(STDIN_FILENO, &buf, 1);
            c = buf[0];
        }
        if (length == 0) {
            //return 0;
            // exit(1);
            kernel_puts("\r\n");
            goto read_prompt;
        } else {
            command[length] = '\0';
            kernel_puts("\r\n");
        }

        x = (char *) &command;
        result = parseSum ();
        if (!errorFlag && !command_was_executed)
        {
            //kprintf ("result = %u\r\n", result);
        }
        errorFlag = false;
        length = 0;
read_prompt:
        ;
    }

    kernel_puts("[\r\n]");
    return 0;
}

/*
size_t kstrlen(const char *t)
{
    size_t ct = 0;
    while (*t++)
        ct++;
    return ct;
}
*/

int get_inode(char *s)
{
    //ext2_inode my_inode;
    uint32_t inode_number = 0;
    inode_number = kernel_strtoul(s, NULL, 10);
//    kprintf("get_inode(%s)[%lu]\r\n", s, inode_number);
    assert(ext2_inode_lookup(inode_number, &my_inode, true));
    return 1;
}

int select_disk(char *s)
{
    int probe = 0;
    int disk_number = 0;
    int device_number = 0;
    disk_number = kernel_strtoul(s, NULL, 10);
    if (!(disk_number >=0 && disk_number <= 0x0F)) {
        kprintf("invalid disk number %u\r\n", disk_number);
        return 0;
    }

    device_number = dev_getdisknumber(disk_number);
    disk_set_drive(device_number);
    bdos_set_drive(device_number);
    //disk_set_dma(0x8000);
    /* should be e2fs superblock @ byte 0x400 (1024) */
    //disk_read_sector(8);
    //dump("0x08000");
    probe = ext2_probe();
    if (probe == -1) {
        kprintf("No filesystem found on disk #%02u\n", disk_number);
    }
    return 0;
}


mathRegister
parseSum ()
{
//   kprintf("parseSum(%s)\r\n", x);
    mathRegister pro1 = parseProduct ();
    while (*x == '+')
    {
        mathRegister pro2;
        ++x;
        pro2 = parseProduct ();
        pro1 = pro1 + pro2;
    }
//    kprintf("returning = %u\r\n", pro1);
    return pro1;

}

mathRegister
parseProduct ()
{

//    kprintf("parseProduct(%s)\r\n", x);
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
    char *search_cmd = NULL;
    int elf_ok = 0;
    // kprintf("parseFactor(%s)\r\n", x);
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

        if (!kstrlen (parseString))
        {
            errorFlag = true;
            return 0;
        }

        while (jumpPtr > 0)
        {
            //kprintf("%u: %s ? %s\n", jmpIndex, jmptbl[jmpIndex].command, parseString);
            if (kernel_strncmp
                    (jmptbl[jmpIndex].command, parseString,
                     kstrlen (jmptbl[jmpIndex].command)) == 0 &&
                    kstrlen (jmptbl[jmpIndex].command) == kstrlen(parseString))
            {
                command_was_executed = true;
                return jumpPtr (x);
            }
            jmpIndex++;
            jumpPtr = jmptbl[jmpIndex].cmdptr;
        }

        /* finally, search the root directory for the command */

        if (parseString[0] != 0x2F) {
            search_cmd = search_path(parseString);
        } else {
            search_cmd = parseString;
        }
//       kprintf("search_cmd=[%s]\r\n", search_cmd);
        if (search_cmd) {
//            kprintf("found executable=[%s], args=[%s]\r\n", search_cmd, x);
            elf_ok = loadelf(search_cmd);
            if (! elf_ok) {
                //kprintf("%s: not an ELF executable\r\n", parseString);
                //kprintf("[ CAUTION: loading binary program '%s' in legacy mode ]\r\n", parseString);
                //kernel_puts("\r\n");
                //load(parseString);
                kprintf("%s: cannot exec binary file\r\n", parseString);
                return 0;
            } else {
                //kprintf("[STARTING ELF PROGRAM]\r\n");
                return run(x);
            }
        }

        kprintf ("shim: %s: command not found ...\r\n", parseString);
        errorFlag = true;
        return 0;
    }
}


mathRegister
parseToken ()
{
    int i = 0;
    //char *p = x;
    //  kprintf("parseToken(%s)\r\n", x);
    kernel_memset (&parseString, 0, MAX_STRING);
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
//   kprintf(" > isTerminator(%c) = %d\r\n", c, c);
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
//    kprintf(" > isWhitespace(%c) = %d\r\n", c, c);
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
    //kprintf("cpm_seek(d->%s, 0x%08lx)\r\n", d->name, address);
    /* not aligned to sector boundary */
    //assert(!(address % SECTOR_SIZE));
    d->offset = address;
    return 0;

}

int cpmsim_write(struct _device *d, unsigned char *buf, uint32_t size)
{
    uint32_t start_sector = 0;
    uint32_t end_sector = 0;
    uint32_t current_sector = 0;
    uint32_t sector_offset = 0;
    uint32_t sector_count = 0;
    uint32_t remaining = size;
    //uint32_t byte_read_count = 0;
    uint32_t current_offset = 0;
    uint32_t bytes_available = 0;
    unsigned char *ptr = buf;
    //kprintf("cpm_write(d->%s, [0x%08lx]->0x%08lx, %lu)\r\n", d->name, d->offset, (uint32_t) buf, size);

    start_sector = d->offset / SECTOR_SIZE;
    sector_offset = d->offset % SECTOR_SIZE;
    sector_count = (size / SECTOR_SIZE) + (size % SECTOR_SIZE ? 1 : 0);
    end_sector = start_sector + sector_count;
    //kprintf("->(sector=%u:offset=%u:sector_count=%u:end_sector=%u)\r\n", start_sector, sector_offset, sector_count, end_sector);
    // assert(!(d->offset % SECTOR_SIZE));
    disk_set_dma(0xF000);
    /* can't read partial head block right now */
    //assert(!sector_offset);

    current_sector = start_sector;
    current_offset = sector_offset;

    while (remaining > 0) {
        if (remaining <= current_offset) {
            /* not sure why this was important before */
            //kprintf("remaining(%u) <= current_offset(%u)\r\n", remaining, current_offset);
            //assert(remaining > current_offset);
        }

        // assert(remaining > current_offset);
        assert(current_offset <= SECTOR_SIZE);
        bytes_available = SECTOR_SIZE - current_offset;
        if (bytes_available > remaining) {
            bytes_available = remaining;
        }
        disk_set_dma(ptr);
        //kernel_memcpy((const void *) 0xF000 + current_offset, ptr, bytes_available);
        disk_write_sector(current_sector);
        //kernel_memcpy((const void *) 0xF000 + current_offset, ptr, bytes_available);
        remaining -= bytes_available;
        ptr+= bytes_available;
        current_offset = 0;
        current_sector++;
    }
    return 0;
}


int cpmsim_read(struct _device *d, unsigned char *buf, uint32_t size)
{
    uint32_t start_sector = 0;
    uint32_t end_sector = 0;
    uint32_t current_sector = 0;
    uint32_t sector_offset = 0;
    uint32_t sector_count = 0;
    uint32_t remaining = size;
    //uint32_t byte_read_count = 0;
    uint32_t current_offset = 0;
    uint32_t bytes_available = 0;
    unsigned char *ptr = buf;
    //kprintf("cpm_read(d->%s, [0x%08lx]->0x%08lx, %lu)\r\n", d->name, d->offset, (uint32_t) buf, size);

    start_sector = d->offset / SECTOR_SIZE;
    sector_offset = d->offset % SECTOR_SIZE;
    sector_count = (size / SECTOR_SIZE) + (size % SECTOR_SIZE ? 1 : 0);
    end_sector = start_sector + sector_count;
    //kprintf("->(sector=%u:offset=%u:sector_count=%u:end_sector=%u)\r\n", start_sector, sector_offset, sector_count, end_sector);
    // assert(!(d->offset % SECTOR_SIZE));
    disk_set_dma(0xF000);
    /* can't read partial head block right now */
    //assert(!sector_offset);

    current_sector = start_sector;
    current_offset = sector_offset;



    while (remaining > 0) {
        if (remaining <= current_offset) {
            /* not sure why this was important before */
            //kprintf("remaining(%u) <= current_offset(%u)\r\n", remaining, current_offset);
            //assert(remaining > current_offset);
        }

        // assert(remaining > current_offset);
        assert(current_offset <= SECTOR_SIZE);
        bytes_available = SECTOR_SIZE - current_offset;
        if (bytes_available > remaining) {
            bytes_available = remaining;
        }
        disk_read_sector(current_sector);
        kernel_memcpy(ptr, (const void *) 0xF000 + current_offset, bytes_available);
        remaining -= bytes_available;
        ptr+= bytes_available;
        current_offset = 0;
        current_sector++;
    }
    return 0;
}

int ls(char *s)
{
    //kprintf("ls(%s)\r\n", s);
    if (!ext2_rootfs.active) {
        kprintf("error: no active filesystem\r\n");
        kernel_puts("\r\n");
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
    //kprintf("cd(%s)\r\n", s);
    if (!ext2_rootfs.active) {
        kprintf("error: no active filesystem\r\n");
        kernel_puts("\r\n");
        return 0;
    }
    assert(ext2_rootfs.active);
    assert(ext2_rootfs.cwd_inode);
    target_inode = ext2_path_to_inode(s, ext2_rootfs.cwd_inode);
    //kprintf("cd: target inode = %u\r\n", target_inode);
    if (!target_inode) {
        kprintf("%s: directory does not exist\r\n", s);
        return 0;
    }
    switch(isdirectory(target_inode)) {
    case true:
        kprintf("[changed directory to %s]\r\n", s);
        ext2_rootfs.cwd_inode = target_inode;
        break;
    case false:
        kprintf("%s: not a directory\r\n", s);
        return 0;
        break;
    default:
        kprintf("huh?\r\n");
        break;
    }
    return 0;
}

int cat(char *s)
{

    char buffer[4096];
    int rd = 0;
    int i = 0;
    int cat_fd = 0;
    cat_fd = kopen(s, O_RDONLY);

    if (cat_fd == -1) {
        perror("open");
        return 0;
    }

    kernel_memset(&buffer, 0, 4096);
    rd = kread(cat_fd, &buffer, 4096);

    while (rd != 0) {
        if (rd == -1) {
            perror("read");
            kernel_puts("\r\n");
            return 0;
        } else {
            //kprintf("rd = %d\r\n", rd);
            for (i = 0; i < rd; i++) {
                kernel_putchar(buffer[i]);
            }
        }
        kernel_memset(&buffer, 0, 4096);
        rd = kread(cat_fd, &buffer, 4096);
    }
    //kprintf("*rd = %d\r\n", rd);

    if (kclose(cat_fd) == -1) {
        perror("close");
        return 0;
    };

    kernel_puts("\r\n");
    return 0;
}

int load(char *s)
{

    int rd = 0;
    int load_fd = 0;
    //void *memptr = (void *) 0x100000;
    void *memptr = (void *) 0x2000000;

    load_fd = kopen(s, O_RDONLY);

    if (load_fd == -1) {
        perror("open");
        return 0;
    }

    rd = kread(load_fd, memptr, 4096);

    while (rd != 0) {
        if (rd == -1) {
            perror("read");
            kernel_puts("\r\n");
            return 0;
        } else {
            //       kprintf("rd = %d\r\n", rd);
            //       kernel_puts("\r\n");
            memptr +=rd;
        }
        rd = kread(load_fd, memptr, 4096);
    }

    if (kclose(load_fd) == -1) {
        perror("kclose");
        return 0;
    };

    return 0;
}

bool iswhitespace(char c)
{
    if (c == 32 || c == '\t') {
        return true;
    }

    return false;
}

int run(char *s)
{

    //kprintf("run(->%s, %s)\r\n", parseString, s);
    return exec_run(parseString, s);
}

int exec_run(char *pname, char *s)
{

    int i = 0;
    int argc = 0;
    int al = 0;
    //int argcount = 0;
    char *ap = NULL;
    char *args[MAX_ARGS];
    int c = 0;
    //kprintf("args = [%s]\r\n", s);

    for (i = 0; i < MAX_ARGS; i++) {
        args[i] = NULL;
    }

    args[0] =  ( pname ? pname : "unknown" );
    argc++;

    ap = s;
    al = 0;
    i = 0;
    while (s[i] != '\0' && s[i] != '\n' && s[i] != '\r' && argc < MAX_ARGS) {
        if (iswhitespace(s[i])) {
            if (al) {
                args[argc] = ap;
                s[i] = '\0';
                //kprintf("new_arg[%d:%s]\r\n", argc, args[argc]);
                argc++;
                al = 0;
            }
        } else {
            if (!al) {
                ap = s + i;
            }
            // kprintf("%d:%d:%c\r\n", i, al, s[i]);
            al++;
        }
        i++;
    }

    if (al) {
        args[argc] = ap;
        //kprintf("last_arg[%d:%s]\r\n", argc, args[argc]);
        argc++;
    }


    for (i = 0; i < argc; i++) {
//        kprintf("args[%d] = [%s]\r\n", i, args[i]);
    }


    //int (*newmain)(_bdos_vtable *btvt, int argc, char *argv[]);
    int (*newmain)(int argc, char *argv[]);
    newmain = (void *) 0x100000;
    //c = newmain(&bdvt, 3, args);
    c = newmain(argc, args);
    //kprintf("[program returned %d]\r\n", c);
    return c;

}

char *search_path(char *s)
{
    static char pathbuf[MAX_PATH];
    struct stat statbuf;
    char *p1 = NULL, *p2 = NULL, *p3 = NULL, *p4 = NULL;
    uint16_t p = 0;
    uint16_t pl = 0;
    uint16_t pg = 0;
    int sb = 0;
    char *path = kgetenv("PATH");

    if (path) {
        pl = kstrlen(path);
        //kprintf("Searching PATH (length=%u): %s\r\n", pl, path);
        p1 = (char *) path;
        p2= (char *) p1;
        kernel_memset(&pathbuf, 0, MAX_PATH);
        while (pg < pl) {
            while (p2[0] != ':'  && p < pl) {
                pathbuf[p] = p2[0];
                p2++;
                p++;
                pg++;
            }
            //kprintf("Got path segment: %s\r\n", pathbuf);
            p2++;
            p = 0;

            if ((kstrlen(s) + kstrlen(pathbuf) + 1) < MAX_PATH) {
                sb = kernel_stat(&pathbuf, &statbuf);

                if (sb == 0 && S_ISDIR(statbuf.st_mode)) {
//										kprintf(" ++ [%s] is a directory\r\n", pathbuf);
                    assert(kstrlen(pathbuf) < MAX_PATH);
//										if (kstrlen(pathbuf) < MAX_PATH) {
//		                    strcat(&pathbuf, "/");
                    //											}
                    kernel_strncat((char *) &pathbuf, "/", kstrlen("/"));
                    kernel_strncat((char *) &pathbuf, s, kstrlen(s));
                    p4 = (char *) &pathbuf;

                    if (kstrlen(p4) > 1) {
                        while (p4[0] == 0x2F && p4[1] == 0x2F && kstrlen(p4) >= 2) {
                            p4++;
                        }
                    }

//                    kprintf(" >> searching [%s]\r\n", p4);
                    if (ext2_path_to_inode((char *) p4, ext2_rootfs.cwd_inode)) {
//                        kprintf("  >> found [%s]\r\n", p4);
                        p3 = (char *) p4;
                        return (char *) p3;
                    } else {
//											kprintf("  >> not found [%s]\r\n", p4);
                    }
                } else {
//                    kprintf(" >> ignoring [%s], not found or not a directory\r\n", pathbuf);
                }

            } else {
                kprintf("(concatenated path would overflow)\r\n");
                return(NULL);
            }
            p = 0;
            kernel_memset(&pathbuf, 0, MAX_PATH);
        }

    }

    /*
    if (ext2_path_to_inode(s, ext2_rootfs.cwd_inode)) {
    return 1;
    }
    */

    return NULL;
}



int run_f16(char *s)
{
    uint32_t addr = 0;
    uint32_t len = 0;
    uint16_t result = 0;
    char *p = NULL;
    x = s;

    addr = parseFactor();
    while (iswhitespace(x[0])) {
        x++;
    }
    len = parseFactor();
    p = (char *) addr;
    result = fletcher16((unsigned char *) p, len);
    kprintf("[f16:addr = 0x%lx, len = 0x%lx, result = 0x%x]\r\n", addr, len, result);
    return 0;
}

int run_f16f(char *s)
{
    //ext2_inode i;
    uint32_t inode = 0;
    uint16_t result = 0;

    inode = ext2_path_to_inode(s, ext2_rootfs.cwd_inode);
    if (!inode) {
        kprintf("f16f: %s not found\r\n", s);
        return 0;
    }

    if (isdirectory(inode)) {
        kprintf("f16: %s is a directory\r\n", s);
        return 0;
    }
    ext2_inode_lookup(inode, &my_inode, false);
    kprintf("%s->i_size = %lu\r\n", s, nm_uint32(my_inode.i_size));
    load(s);
    result = fletcher16((unsigned char *) 0x100000, nm_uint32(my_inode.i_size));
    kprintf("[f16:addr = 0x%lx, len = 0x%lx, result = 0x%x]\r\n", (uint32_t) 0x10000, nm_uint32(my_inode.i_size), result);
    return 0;

}

int quit(char *s)
{
    exit(0);
    /* never reached - here to satisfy compiler */
    return(0);
}
