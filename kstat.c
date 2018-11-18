#include <string.h>
#include <assert.h>

int kstat(const char *restrict path, struct stat *restrict buf)
{
	printf("kstat(%s, 0x%lx)\r\n", path, buf);
	while (1) { } 
}
