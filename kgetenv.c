#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "environ.h"

extern char environment[MAX_ENVIRON];

char *kgetenv(const char *name)
{
    char *p = &environment;
    uint32_t env_offs = 0;
    uint32_t ev_len = 0;
    assert(name);
    while (p[0] != NULL) {
        ev_len = strlen(p);
        if (ev_len < MAX_ENVIRON) {
            if (strncmp(name, p, strlen(name)) == 0 && p[strlen(name)] == '=') {
                return (char *) (p + strlen(name) + 1);
            }
        }
        p+= (ev_len + 1);
    }
    return NULL;
}


