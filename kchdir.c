#define __BDOS__

#include "stdio.h"
#include "stddef.h"
#include <string.h>
#include <sys/types.h>
#include "fcntl.h"
#include "assert.h"
#include "errno.h"
#include "byteorder.h"
#include "ext2.h"

extern ext2_fs ext2_rootfs;


int kchdir(char *s)
{   
    uint32_t target_inode = 0;
    printf("kchdir(%s)\r\n", s);
    if (!ext2_rootfs.active) {
        printf("error: no active filesystem\r\n");
        puts("\r\n");
        return 0;
    }
    assert(ext2_rootfs.active);
    assert(ext2_rootfs.cwd_inode);
    target_inode = ext2_path_to_inode(s, ext2_rootfs.cwd_inode);
    //printf("cd: target inode = %u\r\n", target_inode);
    if (!target_inode) {
        printf("%s: directory does not exist\r\n", s);
				set_errno(ENOENT);
        return 0;
    }
    switch(isdirectory(target_inode)) {
    case true:
        printf("[kchdir changed directory to %s]\r\n", s);
        ext2_rootfs.cwd_inode = target_inode;
        break;
    case false:
				set_errno(ENOTDIR);
        printf("%s: not a directory\r\n", s);
        return 0;
        break;
    default:
        printf("huh?\r\n");
        break;
    }
    return 0;
}

