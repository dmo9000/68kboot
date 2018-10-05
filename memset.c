#include "types.h"

void *memset(void *s, int c, size_t n)
{
	int i = 0;
	unsigned char *p = s;

	for (i = 0; i < n; i++) {
		p[0] = c;
		p++;
		}
	return s;
}
