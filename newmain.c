#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main(int argc, char *argv[])
{

    int i = 0;
    printf("Hello world!\r\n");
    puts("\r\n");
    printf("5 + 5 = %u\r\n", 5+ 5);
    puts("\r\n");
    printf("received %d arguments\r\n", argc);
    puts("\r\n");
    return 0;

}




