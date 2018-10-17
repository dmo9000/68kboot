#include "stdio.h"

extern int main(int argc, char *argv[]);

int _start(int argc, char *argv[])
{

    printf("*** _start() ***\r\n");
    puts("\r\n");
    puts("\r\n");
    return main(argc, argv);

}
