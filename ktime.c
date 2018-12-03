#include <string.h>
#include <time.h>

#define S_TIME  0xff7ff8

time_t ktime(time_t *tloc)
{
    time_t *t  = 0;
    time_t t1 = 0;
    char *tstr = NULL;

    t = S_TIME;
    t1 = *t;

    if (tloc) {
        tloc[0] = t[0];
        tloc[1] = t[1];
        tloc[2] = t[2];
        tloc[3] = t[3];
    }

    return t1;
}

