#define __BDOS__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "environ.h"
#include "assert.h"

extern char environment[MAX_ENVIRON];
uint32_t off_ins = 0; 

int kputenv(char *string)
{
	uint32_t env_req = 0;
	uint32_t env_free = MAX_ENVIRON;
	char *p = &environment; 
//	printf("kputenv(%s)\r\n", string);

	/* must contain an equals sign */
	assert(strchr(string, '='));

	env_req = strlen(string);

	if (env_req > (MAX_ENVIRON - off_ins) -1) {
			/* not enough free space */
			set_errno(ENOMEM);
			return -1;
			}

	/* TODO: check that it doesn't already exist in environment */
	
	memcpy(p + off_ins, string, strlen(string));
	off_ins += strlen(string) + 1;

	return 0;

}

