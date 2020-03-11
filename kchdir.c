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
//    kprintf("kchdir(%s)\r\n", s);
    if (!ext2_rootfs.active) {
//        kprintf("error: no active filesystem\r\n");
//        kernel_puts("\r\n");
        set_errno(EFAULT);
        return -1;
    }

    assert(ext2_rootfs.active);
    assert(ext2_rootfs.cwd_inode);
    target_inode = ext2_path_to_inode(s, ext2_rootfs.cwd_inode);
    //printf("cd: target inode = %u\r\n", target_inode);
    if (!target_inode) {
//        kprintf("%s: directory does not exist\r\n", s);
        set_errno(ENOENT);
        return -1;
    }
    switch(isdirectory(target_inode)) {
    case true:
//        kprintf("[kchdir changed directory to %s]\r\n", s);
        ext2_rootfs.cwd_inode = target_inode;
        set_errno(0);
        return 0;
        break;
    case false:
        set_errno(ENOTDIR);
//        kprintf("%s: not a directory\r\n", s);
        return -1;
        break;
    default:
        kprintf("huh?\r\n");
        break;
    }

    set_errno(EIO);
    return -1;
}

