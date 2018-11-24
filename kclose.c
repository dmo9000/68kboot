#include <unistd.h>
#include "fcntl.h"

int kclose(int fildes)
{

    return fcntl_close(fildes);

}
