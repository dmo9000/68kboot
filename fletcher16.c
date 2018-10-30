
#ifndef __LINUX__
#include "types.h"
#include "fletcher16.h"
#else 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int8_t uint8_t;

#include "fletcher16.h"


int main(int argc, char *argv)
{
    char *text = "Hello world 1\r\n";

    printf("fletcher16 result = 0x%x\r\n", fletcher16(text, strlen(text)));

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
