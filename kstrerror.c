#include "string.h"
#include <errno.h>

const char *kernel_strerror(int en)
{

    switch(en) {
    case 0:
        return (const char *) "Success";
        break;
    case EPERM:
        return (const char *) "Permission denied";
        break;
    case ENOENT:
        return (const char *) "No such file or directory";
        break;
    case EIO:
        return (const char *) "I/O error";
        break;
    case E2BIG:
        return (const char *) "Argument list too long";
        break;
    case EBADF:
        return (const char *) "Bad file descriptor";
        break;
    case EINVAL:
        return (const char *) "Illegal address";
        break;
    case ENFILE:
        return (const char *) "Too many open files";
        break;
    case ENOTTY:
        return (const char *) "Not a typewriter";
        break;
    case EPIPE:
        return (const char *) "Transport endpoint not connected";
        break;
    case EAGAIN:
        return (const char *) "Try again";
        break;
    case ENOTSUPP:
        return (const char *) "Operation not supported";
        break;
    case EISDIR:
        return (const char *) "Is a directory";
        break;
    case ENOTDIR:
        return (const char *) "Not a directory";
        break;
    case EFBIG:
        return (const char *) "File too large";
        break;
    /* FIXME: below here these need to be re-ordered */
    case ENOMEM:
        return (const char *) "Out of memory";
        break;
    case EFAULT:
        return (const char *) "Bad address";
        break;

    }

    return (const char *) "Unknown error!";

}

