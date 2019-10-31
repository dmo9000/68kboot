#include <sys/types.h>
#include <stdbool.h>

bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

uint16_t nm_uint16(uint16_t val)
{
    return (is_big_endian() ? swap_uint16(val) : val);

}

uint32_t nm_uint32(uint32_t val)
{
    return (is_big_endian() ? swap_uint32(val) : val);

}

