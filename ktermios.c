#define __BDOS__

#include <stdio.h>
#include <bdos.h>
#include <termios.h>
#include "errno.h"


TTY ktermios;

int ktermios_init()
{
    kernel_memset(&ktermios, 0, sizeof(TTY));
    //ktermios.c_iflag = (IXANY) | (ICRNL) | (INLCR);
    //ktermios.c_iflag = __builtin_bswap32(ktermios.c_iflag);
    //ktermios.c_iflag = 0x(IXANY);
    ktermios.c_lflag |= ECHO;
    return(0);
}

int ktcgetattr(int fd, struct termios *termios_p)
{
    //kernel_printf("ktcgetattr(%d, 0x%08x)\n", fd, termios_p);
    if (fd > STDERR_FILENO) {
        set_errno(ENOTTY);
        return -1;
    }
    set_errno(SUCCESS);
    kernel_memcpy(termios_p, &ktermios, sizeof(TTY));
    return 0;
}

int ktcsetattr(int fd, int optional_actions, const struct termios *termios_p)
{
    //kernel_printf("ktcsetattr(%d, %d, 0x%08x)\n", fd, optional_actions, termios_p);
    if (fd > STDERR_FILENO) {
        set_errno(ENOTTY);
        return -1;
    }
    kernel_memcpy(&ktermios, termios_p, sizeof(TTY));
    set_errno(SUCCESS);
    return 0;
}

