#define __BDOS__

//#include <string.h>
//#include <assert.h>
#include <sys/stat.h>
//#include "errno.h"
//#include "ext2.h"
//#include "fcntl.h"
//#include <string.h>
//#include "assert.h"
#include "fcntl.h"
#include <string.h>
#include "assert.h"
#include "errno.h"


int kstat(const char *restrict path, struct stat *restrict buf)
{
	ext2_inode my_inode;
	uint32_t directory_inode = 0;
	//printf("kstat(%s, 0x%lx)\r\n", path, buf);
	directory_inode = ext2_path_to_inode(path);
	//printf("inode = %lu\r\n", directory_inode);
	if (!ext2_inode_lookup(directory_inode, &my_inode, false)) {
				set_errno(ENOENT);
				return -1;
				}
	//printf("sizeof struct stat = %u\r\n", sizeof(struct stat));
	//printf("mode = %u\r\n", nm_uint16(my_inode.i_mode));
	//printf("uid = %u\r\n", nm_uint16(my_inode.i_uid));
	//printf("gid = %u\r\n", nm_uint16(my_inode.i_gid));


	memset(buf, 0, sizeof(struct stat));
	buf->st_ino = directory_inode;
	buf->st_mode = nm_uint16(my_inode.i_mode);
	buf->st_uid = nm_uint16(my_inode.i_uid);
	buf->st_gid = nm_uint16(my_inode.i_gid);
	buf->st_size = nm_uint32(my_inode.i_size);
	buf->st_mtime = nm_uint32(my_inode.i_mtime);
	buf->st_atime = nm_uint32(my_inode.i_atime);
	return 0;
}
