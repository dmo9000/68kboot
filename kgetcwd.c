#include "limits.h"
#include "kgetcwd.h"

char cwd[PATH_MAX];


char *kgetcwd(char *buf,size_t len)
{
    uint32_t cwd_inode = 0;
    kernel_printf("kgetcwd()\n\r");
    cwd_inode = ext2_get_cwd_inode();
    kernel_memset(&cwd, 0, PATH_MAX);
    cwd[0] = '?';
    kernel_memcpy(buf, &cwd,  kstrlen(cwd));
    kernel_printf("-> [%s]\n", buf);
    return (char *) &cwd;

}
