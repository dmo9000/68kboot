#ifndef __EXT2_H__
#define __EXT2_h__

#include "types.h"
#include "stdbool.h"
#include "devices.h"

#define EXT2_SUPER_MAGIC    	0xEF53
#define EXT2_MIN_BLOCK_SIZE     1024
#define EXT2_MAX_BLOCK_SIZE	4096
//#define EXT2_BLOCK_SIZE(s)	(EXT2_MIN_BLOCK_SIZE << (s)->s_log_block_size)

#define EXT2_IBUFFER_SIZE   128
//#define EXT2_IBUFFER_SIZE	16384
#define EXT2_DBUFFER_SIZE	65536

#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_IND_BLOCKS         256
#define EXT2_DIND_BLOCK         (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK         (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS           (EXT2_TIND_BLOCK + 1)

#define EXT2_FT_UNKNOWN			0
#define EXT2_FT_FILE			1
#define EXT2_FT_DIR			2

#define EXT2_ROOT_INODE			2

/* file modes and permissions */

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001


#define FILENAME_MAX    256

typedef struct ext2_super_block
{
    uint32_t s_inodes_count;		/* Inodes count */
    uint32_t s_blocks_count;		/* Blocks count */
    uint32_t s_r_blocks_count;	/* Reserved blocks count */
    uint32_t s_free_blocks_count;	/* Free blocks count */
    uint32_t s_free_inodes_count;	/* Free inodes count */
    uint32_t s_first_data_block;	/* First Data Block */
    uint32_t s_log_block_size;	/* Block size */
    int32_t s_log_frag_size;	/* Fragment size */
    uint32_t s_blocks_per_group;	/* # Blocks per group */
    uint32_t s_frags_per_group;	/* # Fragments per group */
    uint32_t s_inodes_per_group;	/* # Inodes per group */
    uint32_t s_mtime;		/* Mount time */
    uint32_t s_wtime;		/* Write time */
    uint16_t s_mnt_count;		/* Mount count */
    int16_t s_max_mnt_count;	/* Maximal mount count */
    uint16_t s_magic;		/* Magic signature */
    uint16_t s_state;		/* File system state */
    uint16_t s_errors;		/* Behaviour when detecting errors */
    uint16_t s_minor_rev_level;	/* minor revision level */
    uint32_t s_lastcheck;		/* time of last check */
    uint32_t s_checkinterval;	/* max. time between checks */
    uint32_t s_creator_os;		/* OS */
    uint32_t s_rev_level;		/* Revision level */
    uint16_t s_def_resuid;		/* Default uid for reserved blocks */
    uint16_t s_def_resgid;		/* Default gid for reserved blocks */
    /* only for dynamic revision (1) */
    uint32_t s_first_ino;         /* first inode */
    uint16_t s_inode_size;         /* first inode */
} ext2_super_block;

typedef struct ext2_group_desc
{
    uint32_t bg_block_bitmap;	/* Blocks bitmap block */
    uint32_t bg_inode_bitmap;	/* Inodes bitmap block */
    uint32_t bg_inode_table;		/* Inodes table block */
    uint16_t bg_free_blocks_count;	/* Free blocks count */
    uint16_t bg_free_inodes_count;	/* Free inodes count */
    uint16_t bg_used_dirs_count;	/* Directories count */
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
} ext2_group_desc;

typedef struct ext2_inode
{
    uint16_t i_mode;			/* File mode */
    uint16_t i_uid;			/* Low 16 bits of Owner Uid */
    uint32_t i_size;			/* Size in bytes */
    uint32_t i_atime;		/* Access time */
    uint32_t i_ctime;		/* Creation time */
    uint32_t i_mtime;		/* Modification time */
    uint32_t i_dtime;		/* Deletion Time */
    uint16_t i_gid;			/* Low 16 bits of Group Id */
    uint16_t i_links_count;		/* Links count */
    uint32_t i_blocks;		/* Blocks count */
    uint32_t i_flags;		/* File flags */
    union
    {
        struct
        {
            uint32_t l_i_reserved1;
        }
        linux1;
        struct
        {
            uint32_t h_i_translator;
        }
        hurd1;
        struct
        {
            uint32_t m_i_reserved1;
        }
        masix1;
    }
    osd1;				/* OS dependent 1 */
    uint32_t i_block[EXT2_N_BLOCKS];	/* Pointers to blocks */
    uint32_t i_generation;		/* File version (for NFS) */
    uint32_t i_file_acl;		/* File ACL */
    uint32_t i_dir_acl;		/* Directory ACL */
    uint32_t i_faddr;		/* Fragment address */
    union
    {
        struct
        {
            uint8_t l_i_frag;		/* Fragment number */
            uint8_t l_i_fsize;		/* Fragment size */
            uint16_t i_pad1;
            uint16_t l_i_uid_high;	/* these 2 fields    */
            uint16_t l_i_gid_high;	/* were reserved2[0] */
            uint32_t l_i_reserved2;
        }
        linux2;
        struct
        {
            uint8_t h_i_frag;		/* Fragment number */
            uint8_t h_i_fsize;		/* Fragment size */
            uint16_t h_i_mode_high;
            uint16_t h_i_uid_high;
            uint16_t h_i_gid_high;
            uint32_t h_i_author;
        }
        hurd2;
        struct
        {
            uint8_t m_i_frag;		/* Fragment number */
            uint8_t m_i_fsize;		/* Fragment size */
            uint16_t m_pad1;
            uint32_t m_i_reserved2[2];
        }
        masix2;
    }
    osd2;				/* OS dependent 2 */
} ext2_inode;

#define EXT2_NAME_LEN 255

typedef struct ext2_dir_entry
{
    uint32_t inode;			/* Inode number */
    uint16_t rec_len;		/* Directory entry length */
    uint8_t name_len;		/* Name length */
    uint8_t file_type;
    char name[EXT2_NAME_LEN];	/* File name */
} ext2_dir_entry;

typedef struct ext2_fs {
    uint32_t block_size;
    struct ext2_super_block blck;
    ext2_group_desc group_descriptor;
    _device *device;
    unsigned int blocks_per_group;
    unsigned int blocks_count;
    unsigned int block_groups;
    unsigned int inodes_per_group;
    unsigned long grdesc_offset;
    unsigned long group_size;
    unsigned long block_bitmap;
    unsigned long block_bitmap_s;
    unsigned long inode_bitmap;
    unsigned long inode_bitmap_s;
    unsigned long inode_table;
    //unsigned char ibuffer[EXT2_IBUFFER_SIZE];
    //unsigned char dbuffer[EXT2_DBUFFER_SIZE];
    unsigned long active;
    uint16_t device_number;
    uint32_t cwd_inode;
} ext2_fs;



int ext2_probe();
struct ext2_inode * ext2_readinode (struct ext2_fs *fs, unsigned long itab, unsigned long i);
int ext2_ls (struct ext2_fs *fs, unsigned long i);
bool isdirectory(uint32_t inode);
uint32_t ext2_path_to_inode(char *path);
int ext2_list_directory(uint32_t directory_inode);
int ext2_inode_lookup(uint32_t inode_lookup, ext2_inode *my_inode, bool debug);
uint32_t ext2_get_inode_block(ext2_inode *e2i, uint32_t file_block_id);
uint32_t ext2_block_read(ext2_fs *fs, uint32_t dma_addr, uint32_t block_id);


#endif /* __EXT2_H__ */
