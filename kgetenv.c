#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "environ.h"

extern char environment[MAX_ENVIRON];

char *matchenv(char *e, char *v) 
{
	assert(e);
	assert(v);
//	printf("[%s] <=> [%s]\r\n", e, v);
	/* if the environment string is shorter than the variable being search for, this can't be it */
	if (strlen(e) < strlen(v)+1) {
				return NULL;
				}
	if (strncmp(e, v, strlen(v)) ==0 && e[strlen(v)] == '=') {
			return e+strlen(v)+1;
			}
	return NULL;
}

char *kgetenv(const char *name)
{
	char *p = &environment;
	uint32_t offset = 0;
	const char *ep = NULL;
	char *ret = NULL;

//	printf("kgetenv(%s)\r\n", name);

	while (offset < (MAX_ENVIRON-1)) {
				ep = p + offset;
				assert(ep);
//				printf("offset = %lu %u [%s]\r\n", offset, strlen(ep), ep);
				if (!strlen(ep)) {
					/* end of environment */
					return NULL;
					}
				ret = matchenv(ep, name);
				if (ret) {
//						printf("MATCHED! 0x%lx %s\r\n", ret, ret);
						return ret;
						} 
				/* skip forward to next environment string, or end of environment */
					offset+=strlen(ep)+1;
				}

	assert(NULL);
}


