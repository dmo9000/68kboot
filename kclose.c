#include <unistd.h>
#include "fcntl.h"

int close(int fildes)
{

    return fcntl_close(fildes);

}
