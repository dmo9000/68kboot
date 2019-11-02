#define __BDOS__

#include <sys/stat.h>
#include "fcntl.h"
#include <string.h>
#include <ext2.h>
#include "kernel.h"
#include "assert.h"
#include "errno.h"

extern ext2_fs ext2_rootfs;

uint32_t ext2_inode_size(uint32_t inode_lookup)
{
    static ext2_inode my_inode;

		kernel_memset(&my_inode, 0, sizeof(ext2_inode));
		if (!ext2_inode_lookup(inode_lookup, &my_inode, false)) {
			kernel_printf("ext2_inode_size() failed\r\n");
			set_errno(ENOENT);
			return 0;
			}
//		printf("my_inode.i_size = %u\r\n", nm_uint32(my_inode.i_size)); 
		return nm_uint32(my_inode.i_size);
}


int kernel_stat(const char *restrict path, struct stat *restrict buf)
{
    ext2_inode my_inode;
    uint32_t directory_inode = 0;
    //printf("kstat(%s, 0x%lx)\r\n", path, buf);
    directory_inode = ext2_path_to_inode(path, ext2_rootfs.cwd_inode);
    //printf("inode = %lu\r\n", directory_inode);
    if (!ext2_inode_lookup(directory_inode, &my_inode, false)) {
        set_errno(ENOENT);
        return -1;
    }
    //printf("sizeof struct stat = %u\r\n", sizeof(struct stat));
    //printf("mode = %u\r\n", nm_uint16(my_inode.i_mode));
    //printf("uid = %u\r\n", nm_uint16(my_inode.i_uid));
    //printf("gid = %u\r\n", nm_uint16(my_inode.i_gid));


    kernel_memset(buf, 0, sizeof(struct stat));
    buf->st_ino = directory_inode;
    buf->st_mode = nm_uint16(my_inode.i_mode);
    buf->st_uid = nm_uint16(my_inode.i_uid);
    buf->st_gid = nm_uint16(my_inode.i_gid);
    buf->st_size = nm_uint32(my_inode.i_size);
    buf->st_mtime = nm_uint32(my_inode.i_mtime);
    buf->st_atime = nm_uint32(my_inode.i_atime);
    return 0;
}
