#define __BDOS__

#include "stdio.h"
#include "bdos.h"
#include "fcntl.h"
#include <unistd.h>
#include "errno.h"
#include <string.h>
#include "kstat.h"
#include "kopen.h"
#include "kread.h"
#include "kwrite.h"
#include "kclose.h"
#include "klseek.h"
#include "kchdir.h"
#include "ktime.h"
#include "kgetenv.h"
#include "kfcntl.h"
#include "shim.h"
#include "environ.h"
#include "termios.h"
#include "kgetcwd.h"

_bdos_vtable bdvt       __attribute__((section(".bdos_vtable")));
extern int main(int argc, char *argv[]);

extern char environment[MAX_ENVIRON];

static bool initialized = false;

int bdos_init()
{
    char *path = NULL;
    char *term = NULL;
    if (!initialized) {
        kernel_memset((void *) 0x0400, 0, 256);
        bdvt.magic = 0xF0E0F0E0;
        bdvt.ver_maj = VERSION_MAJOR;
        bdvt.ver_min = VERSION_MINOR;
        bdvt.ver_rev = VERSION_REVISION;
        bdvt._open = kopen;
        bdvt._read = kread;
        bdvt._write = kwrite;
        bdvt._close = kclose;
        bdvt._stat = kernel_stat;
        bdvt._lseek = klseek;
        bdvt._chdir = kchdir;
        bdvt._getcwd = kgetcwd;
        bdvt._time = ktime;
        bdvt._getenv = kgetenv;
        bdvt._fcntl = kfcntl;
        bdvt._tcgetattr = ktcgetattr;
        bdvt._tcsetattr = ktcsetattr;

        kernel_memset(&environment, 0, MAX_ENVIRON);
        //snprintf(&environment, 1024, "PATH=/usr/bin/:/bin/\nTERM=ansi\n");
        kputenv("PATH=/usr/bin:/bin");
        kputenv("TERM=ansi");
        bdos_version(NULL);

        ktermios_init();

        initialized = true;



        dev_register("C:", DEVTYPE_BLOCK, DEV_CPMIO, 4, 0x0, 0x0, cpmsim_seek, cpmsim_read, cpmsim_write);
        select_disk("0");
        path = kgetenv("PATH");
        kernel_puts("\n\r");
        if (path) {
            kprintf("PATH=%s\n\r", path);
        }
        term = kgetenv("TERM");
        if (term) {
            kprintf("TERM=%s\n\r", term);
        }

        kernel_puts("\n\r");
    }
    return 1;
}

int bdos_version(char *s)
{
    kprintf("\n\r68000 BDOS %u.%u.%u\n\r(c) Copyright 2018-2020 - all rights reserved.\n\r\n\r", bdvt.ver_maj, bdvt.ver_min, bdvt.ver_rev);
    //printf("main = 0x%lx\n\r", main);
    //puts("\n\r");
    return 1;
}

int set_errno(int d)
{
    errno = d;
    bdvt.errno = d;
    return bdvt.errno;
}

int bdos_set_drive(uint8_t d)
{
    bdvt.selected_drive = d;
    return 0;
}

bool bdos_initialized()
{
    return initialized;
}
