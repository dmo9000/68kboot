#define __BDOS__

#include <bdos.h>
#include <termios.h>
#include "errno.h"

int ktcgetattr(int fd, struct termios *termios_p)
{
		kernel_printf("ktcgetattr(%d, 0x%08x)\n", fd, termios_p);
		set_errno(ENOTSUPP);
		return -1;
}

int ktcsetattr(int fd, int optional_actions, const struct termios *termios_p)
{
		kernel_printf("ktcsetattr(%d, %d, 0x%08x)\n", fd, optional_actions, termios_p);
		set_errno(ENOTSUPP);
		return -1;
}

