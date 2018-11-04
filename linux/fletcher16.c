
#ifndef __LINUX__
#include "types.h"
#include "fletcher16.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>


typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int8_t uint8_t;

#include "fletcher16.h"


int main(int argc, char *argv[])
{
		char *text = NULL;
		struct stat sbuf;
		int c = 0, fd = 0, rd =0;
		uint32_t total = 0;
		char *p = NULL, *q = NULL;

		for (int i = 0; i < argc; i++) {
			printf("argv[%d] = '%s'\n", i, argv[i]);
		}
		assert(argc == 2);
		assert(argv[1]);
		c = stat(argv[1], &sbuf);
		assert(stat(argv[1], &sbuf) == 0 );
		printf("size = %lu\r\n", sbuf.st_size);
		p = malloc(sbuf.st_size);
		assert(p);
		q = p;
		fd = open(argv[1], O_RDONLY);
		assert(fd != -1);

		rd = read(fd, q, 4096);
		while (rd > 0) {
			total += rd;
			q+=rd;
			rd = read(fd, q, 4096);
			}

		assert(total == sbuf.st_size);
    printf("fletcher16 result = 0x%x\r\n", fletcher16(p, total));

		close(fd);
    exit(0);
}

#endif

uint16_t
fletcher16(const uint8_t *data, size_t len)
{
        uint32_t c0, c1;
        unsigned int i;

        for (c0 = c1 = 0; len >= 5802; len -= 5802) {
                for (i = 0; i < 5802; ++i) {
                        c0 = c0 + *data++;
                        c1 = c1 + c0;
                }
                c0 = c0 % 255;
                c1 = c1 % 255;
        }
        for (i = 0; i < len; ++i) {
                c0 = c0 + *data++;
                c1 = c1 + c0;
        }
        c0 = c0 % 255;
        c1 = c1 % 255;
        return (c1 << 8 | c0);
}
