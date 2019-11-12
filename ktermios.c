#define __BDOS__

#include <stdio.h>
#include <bdos.h>
#include <termios.h>
#include "errno.h"


TTY ktermios;

int ktermios_init()
{
	kernel_printf("ktermios_init()\n\r");
	kernel_memset(&ktermios, 0, sizeof(TTY));	
	return(0);
}



int ktcgetattr(int fd, struct termios *termios_p)
{
		kernel_printf("ktcgetattr(%d, 0x%08x)\n", fd, termios_p);
		if (fd > STDERR_FILENO) {
				set_errno(ENOTTY);
				return -1;
				}
		return -1;
}

int ktcsetattr(int fd, int optional_actions, const struct termios *termios_p)
{
		kernel_printf("ktcsetattr(%d, %d, 0x%08x)\n", fd, optional_actions, termios_p);
		set_errno(ENOTSUPP);
		return -1;
}

