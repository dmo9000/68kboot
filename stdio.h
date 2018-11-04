#include "stddef.h"
#include "stdbool.h"
#include "types.h"


struct  _iobuf {
    char *    _ptr;
    int   _cnt;
    char *    _base;
    char   _flags[4];
    int    _file;
    bool  _eof;
    ssize_t _limit;
};

typedef struct _iobuf FILE;

#include "fcntl_private.h"

#define stdin           (&filehandles[0])
#define stdout          (&filehandles[1])
#define stderr          (&filehandles[2])



int printf(const char *format, ...);
int puts(const char *s);
int getchar(void);
int putchar(int c);
int snprintf(char *str, size_t size, const char *format, ...);
void perror(const char *s);

#define SEEK_SET	0
