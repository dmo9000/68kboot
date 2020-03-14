#define __BDOS__

#include <string.h>
#include "stdio.h"
#include "stdbool.h"
#include "kernel.h"
#include <sys/types.h>
#include "devices.h"
#include "ext2.h"
#include "assert.h"
#include "byteorder.h"
#include "errno.h"
#include "dump.h"

unsigned long ext2_base = 0;
extern _device devices[MAX_DEVICES];
extern int device_free;
ext2_fs ext2_rootfs;
bool ext2_backwards = false;

char dir_entry_type[] = {'?', '-', 'd'};


#define panic() assert(!("PANIC"));
uint32_t EXT2_BLOCK_SIZE(ext2_super_block *s)
{

    if (is_big_endian()) {
        return (EXT2_MIN_BLOCK_SIZE >> nm_uint32((s)->s_log_block_size));
    } else {
        return (EXT2_MIN_BLOCK_SIZE << (s)->s_log_block_size);
    }

};


unsigned long
div2 (unsigned long a, unsigned long b)
{

    /* rounded up */

    return (a / b) + ((a % b) > 1);
}


int ext2_probe()
{
    unsigned char buffer[4096];
    unsigned int i = 0, j = 0;
    ext2_super_block *blck = NULL;
    unsigned long offset = 0;
    void *p = &buffer;
    uint32_t used_inode_count = 0;
    uint32_t used_block_count = 0;
    double used_inodes_percent = 0.0;
    double used_blocks_percent = 0.0;

    for (i = 0; i <= device_free ; i++) {
        if (devices[i].type == DEVTYPE_BLOCK) {
            //kprintf("EXT2: probing device %s\r\n", devices[i].name);
            if (devices[i].seek) {
                devices[i].seek(&devices[i], 0);
                devices[i].read(&devices[i], (unsigned char *) &buffer, 4096);
                for (j = 0x0; j < 4096; j++) {
                    blck = (ext2_super_block *) p;

                    if (blck->s_magic == EXT2_SUPER_MAGIC || (is_big_endian() && swap_uint16(blck->s_magic) == EXT2_SUPER_MAGIC)) {
                        //kprintf("EXT2: found superblock magic == 0x%04X\r\n", swap_uint16(blck->s_magic));
                        goto found_rootfs;
                    }
                    p+=1024;
                    offset +=1024;
                }
            } else {
                kprintf("<driver not implemented!>\r\n");
                kernel_puts("\r\n");
            }
        }
    }

    //kprintf("EXT2: no filesystem found\r\n");
    ext2_rootfs.active = 0;
    kprintf("\r\n");
    kernel_puts("\r\n");
    return(-1);

found_rootfs:
    /* make copy of the superblock */

    kernel_memcpy(&ext2_rootfs.blck, blck, sizeof(ext2_super_block));

    /* collect important information about the filesystem */

    ext2_rootfs.device = &devices[i];
    ext2_rootfs.device_number = i;

    kprintf("ext2: found root ext2fs v%lu.%u (%s) (state %s) \r\n      device %u:%u (%s) at offset 0x%08lx (%lu)\r\n",
                  nm_uint32(ext2_rootfs.blck.s_rev_level),
                  nm_uint16(ext2_rootfs.blck.s_minor_rev_level),
                  (nm_uint32(ext2_rootfs.blck.s_rev_level) ? "dynamic" : "legacy"),
                  (nm_uint16(ext2_rootfs.blck.s_state)  == 1 ? "EXT2_VALID_FS" : "EXT2_ERROR_FS"),
                  ext2_rootfs.device->maj, ext2_rootfs.device->min,
                  ext2_rootfs.device->name,
                  offset, offset);

    /*
    kprintf ("ext2: free inodes=%lu/%lu, free blocks=%lu/%lu, block size=%lu, \r\n  first_inode=%lu, inode_size=%u\r\n",
                   nm_uint32(ext2_rootfs.blck.s_free_inodes_count),
                   nm_uint32(ext2_rootfs.blck.s_inodes_count),
                   nm_uint32(ext2_rootfs.blck.s_free_blocks_count),
                   nm_uint32(ext2_rootfs.blck.s_blocks_count),
                   EXT2_BLOCK_SIZE (&ext2_rootfs.blck),
                   nm_uint32(ext2_rootfs.blck.s_first_ino),
                   nm_uint16(ext2_rootfs.blck.s_inode_size)
                  );
    	*/

    used_inode_count = nm_uint32(ext2_rootfs.blck.s_inodes_count) -
                       nm_uint32(ext2_rootfs.blck.s_free_inodes_count);
    used_inodes_percent = (double) (100.0 / nm_uint32(ext2_rootfs.blck.s_inodes_count)) * used_inode_count;
    used_block_count = nm_uint32(ext2_rootfs.blck.s_blocks_count) -
                       nm_uint32(ext2_rootfs.blck.s_free_blocks_count);
    used_blocks_percent = (double) (100.0 / nm_uint32(ext2_rootfs.blck.s_blocks_count)) * used_block_count;

    kprintf ("      blocks=%.02f%%, inodes=%.02f%%\r\n",
                   used_blocks_percent, used_inodes_percent);

    ext2_rootfs.block_size = EXT2_BLOCK_SIZE (&ext2_rootfs.blck);
    ext2_rootfs.blocks_per_group = nm_uint32(ext2_rootfs.blck.s_blocks_per_group);
    ext2_rootfs.blocks_count  = nm_uint32(nm_uint32(ext2_rootfs.blck.s_blocks_count));
    ext2_rootfs.block_groups = div2(nm_uint32(ext2_rootfs.blocks_count),
                                    nm_uint32(ext2_rootfs.blocks_per_group));
    ext2_rootfs.inodes_per_group = nm_uint32(ext2_rootfs.blck.s_inodes_per_group);

    /*
    kprintf("ext2: blocks per group=%u, inodes per group=%u, block groups=%u\n\r",
              ext2_rootfs.blocks_per_group,
              ext2_rootfs.inodes_per_group,
              ext2_rootfs.block_groups
             );
    */

    if (ext2_rootfs.block_groups != 1) {
        kprintf("FATAL: ext2_rootfs.block_groups != 1\n\r");
        while (1) { }
    }

    if (ext2_rootfs.block_size != 1024) {
        kprintf("FATAL: ext2_rootfs.block_size != 1024\n\r");
        while (1) { }
    }

    assert(nm_uint16(ext2_rootfs.blck.s_inode_size) == 128);
    assert(ext2_rootfs.block_size == 1024);

    if ((offset + ext2_rootfs.block_size) > 4096) {
        kprintf("EXT2: seek required!!\n");
        panic();
    } else {
        p += ext2_rootfs.block_size;
        offset += ext2_rootfs.block_size;
    }

    /* we should now be looking at the group descriptor block */

//    kprintf("EXT2: group descriptor (gr_desc) offset = 0x%x (%u)\r\n", offset, offset);


    ext2_rootfs.group_size =
        (sizeof(ext2_group_desc) * ext2_rootfs.block_groups);
//    kprintf("EXT2: group_size = %u\r\n", ext2_rootfs.group_size);
    //block_offset = div2(ext2_rootfs.group_size, ext2_rootfs.block_size);
//    kprintf("EXT2: block_offset = 0x%08lx\r\n", block_offset);

    /* FIXME: group_descriptor is not always at offset 2048 */
    assert(offset == 0x0800);

    //  kprintf("EXT2: reading %u bytes for group descriptor\r\n", sizeof(ext2_group_desc));
    kernel_memset(&ext2_rootfs.group_descriptor, 0, sizeof(ext2_group_desc));
    devices[i].seek(&devices[i], offset);
    devices[i].read(&devices[i], (unsigned char *) &ext2_rootfs.group_descriptor, sizeof(ext2_group_desc));

    ext2_rootfs.block_bitmap = nm_uint32(ext2_rootfs.group_descriptor.bg_block_bitmap);
    ext2_rootfs.inode_bitmap = nm_uint32(ext2_rootfs.group_descriptor.bg_inode_bitmap);
    ext2_rootfs.inode_table = nm_uint32(ext2_rootfs.group_descriptor.bg_inode_table);

        kprintf("EXT2: bg_block_bitmap = 0x%08lx (0x%08lx)\r\n", ext2_rootfs.block_bitmap, ext2_rootfs.block_bitmap * ext2_rootfs.block_size);
        kprintf("EXT2: bg_inode_bitmap = 0x%08lx (0x%08lx)\r\n", ext2_rootfs.inode_bitmap, ext2_rootfs.inode_bitmap * ext2_rootfs.block_size);
		/*
        kprintf("EXT2: bg_inode_table  = 0x%08lx (0x%08lx)\r\n", ext2_rootfs.inode_table, ext2_rootfs.inode_table * ext2_rootfs.block_size);
        kprintf("EXT2: directory_count = 0x%08lx\r\n", nm_uint16(ext2_rootfs.group_descriptor.bg_used_dirs_count));
    */
    ext2_rootfs.active = 1;
    ext2_rootfs.cwd_inode = EXT2_ROOT_INODE;

    return 1;
}

int ext2_list_directory(uint32_t directory_inode)
{
    char fnbuf[FILENAME_MAX];
    int i = 0;
    ext2_dir_entry current_entry;
    ext2_inode my_inode;
    ext2_inode iter_inode;
    uint32_t directory_address = 0;
    uint32_t directory_offset = 0;
    //uint16_t dirent_size = 0;
    uint32_t inode_index = 0;
    unsigned char perms[12];

    //dirent_size = sizeof(ext2_dir_entry);
//    kprintf("ext2_list_directory(%u)\r\n", directory_inode);
//    kprintf("sizeof(ext2_dir_ent) == %u\r\n", dirent_size);
    assert(ext2_inode_lookup(directory_inode, &my_inode, false));
    /* check inode is a directory */
    assert((nm_uint16(my_inode.i_mode) & 0xE000) == 0x4000);
    /* check that the inode fits in one block for now - we'll handle longer directory listings later */
//    kprintf("nm_uint32(my_inode.i_size) = %u\r\n", nm_uint32(my_inode.i_size));
    //assert((nm_uint32(my_inode.i_size) <= 1024));
    /* check that the direct blocks list has exactly one block */
    //assert(!(my_inode.i_block[1]));
    assert(my_inode.i_block[0]);
    directory_address = (nm_uint32(my_inode.i_block[i]) * ext2_rootfs.block_size);
    //kprintf("directory data at 0x%08lx ... \r\n", directory_address);

    directory_offset = 0;
    while (directory_offset <= 1024) {
        //kprintf("%u[0x%08lx]: \r\n", inode_index, directory_address + directory_offset);
        /* clear the dirent structure */
        kernel_memset(&current_entry, 0, sizeof(ext2_dir_entry));
        devices[ext2_rootfs.device_number].seek(&devices[ext2_rootfs.device_number], directory_address + directory_offset);
        devices[ext2_rootfs.device_number].read(&devices[ext2_rootfs.device_number], (unsigned char *) &current_entry, sizeof(ext2_dir_entry));
        /* if it's zero we're at the end of the directory listing */
        assert(current_entry.inode);

        assert(current_entry.file_type < 3);
        assert(ext2_inode_lookup(nm_uint32(current_entry.inode), &iter_inode, false));

        /*
            if (nm_uint16(current_entry.rec_len) == 0) {
                return 1;
            }
        */

        kernel_memset(&perms, 0, 12);
        kernel_memset(&perms, '-', 10);
        perms[0] = dir_entry_type[current_entry.file_type];
        if (nm_uint16(iter_inode.i_mode) & S_IRUSR) perms[1] = 'r';
        if (nm_uint16(iter_inode.i_mode) & S_IWUSR) perms[2] = 'w';
        if (nm_uint16(iter_inode.i_mode) & S_IXUSR) perms[3] = 'x';

        if (nm_uint16(iter_inode.i_mode) & S_IRGRP) perms[4] = 'r';
        if (nm_uint16(iter_inode.i_mode) & S_IWGRP) perms[5] = 'w';
        if (nm_uint16(iter_inode.i_mode) & S_IXGRP) perms[6] = 'x';

        if (nm_uint16(iter_inode.i_mode) & S_IROTH) perms[7] = 'r';
        if (nm_uint16(iter_inode.i_mode) & S_IWOTH) perms[8] = 'w';
        if (nm_uint16(iter_inode.i_mode) & S_IXOTH) perms[9] = 'x';

        kernel_memset(&fnbuf, 0, FILENAME_MAX);
        assert(current_entry.name_len < FILENAME_MAX);
        kernel_memcpy(&fnbuf, &current_entry.name, current_entry.name_len);

        /*
        kprintf("%010u:0x%08lx: %08u-> %8u %10ld %s %5u %5u %8lu %s%c\r\n",
               inode_index, directory_address + directory_offset,
               current_entry.name_len,
               nm_uint16(current_entry.rec_len),
               nm_uint32(current_entry.inode),
               perms,
               nm_uint16(iter_inode.i_uid), nm_uint16(iter_inode.i_gid),
               nm_uint32(iter_inode.i_size),
               fnbuf,
               (perms[0] == 'd' ? '/' : '\0'));
        */

        /* FIXME: ANSI codes in an ext2 drivers, lol */

        kprintf("%10ld %s %5u %5u %8lu %s%c\r\n",
                      nm_uint32(current_entry.inode),
                      perms,
                      nm_uint16(iter_inode.i_uid), nm_uint16(iter_inode.i_gid),
                      nm_uint32(iter_inode.i_size),
                      fnbuf,
                      (perms[0] == 'd' ? '/' : '\0'));

        kprintf("%c[37m", 27);

        if (nm_uint16(current_entry.rec_len) > sizeof(ext2_dir_entry)) {
            /* looks like we have reached the end of the directory */
            return 1;
        }
        inode_index ++;
        directory_offset += nm_uint16(current_entry.rec_len);
    }

    kprintf("too many entries\r\n");
    return 0;

}

int ext2_inode_lookup(uint32_t inode_lookup, ext2_inode *my_inode, bool debug)
{
    int  i = 0;
    uint32_t inode_block_group = 0;
    uint32_t inode_index = 0;
    //uint32_t containing_block = 0;
    uint32_t inode_offset = 0;

    if (debug) {
        kprintf("ext2_inode_lookup(%lu)\r\n", inode_lookup);
    }

    if (!ext2_rootfs.active) {
        //kprintf("error: no active filesystem\r\n");
        //puts("\r\n");
        return 0;
    }
    assert(ext2_rootfs.active);

    if (!inode_lookup) {
        /* if we are asked to lookup inode 0, it is invalid */
        return 0;
    }

    /* see https://wiki.osdev.org/Ext2#Inode_Type_and_Permissions */
    inode_block_group = (inode_lookup - 1) / ext2_rootfs.inodes_per_group;
//    kprintf("block group for inode %u is %u\r\n", inode_lookup, inode_block_group);
    /* must be group zero for now - deal with multiple block groups later */
    if (inode_block_group) {
        kprintf("inode_block_group = %u, inode_lookup = %lu\r\n", inode_block_group, inode_lookup);
        while(1) { }
    }

    assert(!inode_block_group);
    inode_index = (inode_lookup - 1) % ext2_rootfs.inodes_per_group;
//    kprintf("inode_index = %u\r\n", inode_index);
//    containing_block = (inode_index * nm_uint16(ext2_rootfs.blck.s_inode_size)) / ext2_rootfs.block_size;
//    kprintf("containing_block = %u (0x%08lx)\r\n", containing_block,
    //      (ext2_rootfs.inode_table * ext2_rootfs.block_size) + (inode_index * nm_uint16(ext2_rootfs.blck.s_inode_size)));

    inode_offset = (ext2_rootfs.inode_table * ext2_rootfs.block_size) + (inode_index * nm_uint16(ext2_rootfs.blck.s_inode_size));

    devices[ext2_rootfs.device_number].seek(&devices[ext2_rootfs.device_number], inode_offset);
    devices[ext2_rootfs.device_number].read(&devices[ext2_rootfs.device_number], (unsigned char *) my_inode, sizeof(ext2_inode));

    /* see https://wiki.osdev.org/Ext2#Inode_Type_and_Permissions */

    if (!my_inode->i_mode) {
        /* deleted or non-existent */
        kprintf("inode %lu is invalid or deleted\r\n", inode_lookup);
        return 0;
    }

    if (debug) {
        kprintf("i_mode   = 0x%04x\r\n", (uint16_t) nm_uint16(my_inode->i_mode) & 0xE000);
        kprintf("i_mode   = 0x%04x\r\n", (uint16_t) nm_uint16(my_inode->i_mode) & 0x1FFF);
        kprintf("i_uid    = %u\r\n", nm_uint16(my_inode->i_uid));
        kprintf("i_gid    = %u\r\n", nm_uint16(my_inode->i_gid));
        kprintf("i_size   = %lu\r\n", nm_uint32(my_inode->i_size));
        kprintf("i_flags  = %lu\r\n", nm_uint32(my_inode->i_flags));
        kprintf("i_blocks = %lu\r\n", nm_uint32(my_inode->i_blocks));
        kprintf("i_block pointers:\r\n  ");
        for (i = 0; i < EXT2_N_BLOCKS ; i++) {
            if (nm_uint32(my_inode->i_block[i])) {
                kprintf("[%u: %08lx:%08lx] ", i, nm_uint32(my_inode->i_block[i]), nm_uint32(my_inode->i_block[i]) * ext2_rootfs.block_size);
            }
        }
        kprintf("\r\n");
        kernel_puts("\r\n");
    }

    return 1;
}

uint32_t ext2_get_inode_from_dirent(uint32_t search_inode, char *pathelement)
{
    int i = 0;
    ext2_dir_entry current_entry;
    ext2_inode my_inode;
    ext2_inode iter_inode;
    uint32_t directory_address = 0;
    uint32_t directory_offset = 0;
    //uint16_t dirent_size;
    uint32_t inode_index = 0;
    //dirent_size = sizeof(ext2_dir_entry);
//    kprintf("ext2_get_inode_from_dirent(%u, %s)\r\n", search_inode, pathelement);

    assert(ext2_inode_lookup(search_inode, &my_inode, false));
    assert((nm_uint16(my_inode.i_mode) & 0xE000) == 0x4000);
    //assert((nm_uint32(my_inode.i_size) <= 1024));
    //assert(!(my_inode.i_block[1]));
    assert(my_inode.i_block[0]);
    directory_address = (nm_uint32(my_inode.i_block[i]) * ext2_rootfs.block_size);
    directory_offset = 0;
    while (directory_offset <= 1024) {
        //kprintf("%u[0x%08lx]: \r\n", inode_index, directory_address + directory_offset);
        /* clear the dirent structure */
        kernel_memset(&current_entry, 0, sizeof(ext2_dir_entry));
        devices[ext2_rootfs.device_number].seek(&devices[ext2_rootfs.device_number], directory_address + directory_offset);
        devices[ext2_rootfs.device_number].read(&devices[ext2_rootfs.device_number], (unsigned char*) &current_entry, sizeof(ext2_dir_entry));
        /* if it's zero we're at the end of the directory listing */
        assert(current_entry.inode);
        assert(current_entry.file_type < 3);
        assert(ext2_inode_lookup(nm_uint32(current_entry.inode), &iter_inode, false));

        if (nm_uint16(current_entry.rec_len) == 0) {
            /* end of directory */
            return 0;
        }


        if ((current_entry.name_len == kstrlen(pathelement)) &&
                kernel_strncmp(current_entry.name, pathelement, current_entry.name_len) == 0) {
            return nm_uint32(current_entry.inode);
        }

        if (nm_uint16(current_entry.rec_len) > sizeof(ext2_dir_entry)) {
            /* looks like we have reached the end of the directory */
            return 0;
        }
        inode_index ++;
        directory_offset += nm_uint16(current_entry.rec_len);
    }

    kprintf("too many entries\r\n");
    assert(NULL);
    return (0);
}


bool isdirectory(uint32_t inode)
{
    ext2_inode target_inode;
    kernel_memset(&target_inode, 0, sizeof(ext2_inode));
    if (!ext2_rootfs.active) {
        return false;
    }
    assert(ext2_inode_lookup(inode, &target_inode, false));
    //kprintf("isdirectory(%lu) == 0x%04x?\r\n", inode, (uint16_t) nm_uint16(target_inode.i_mode) & 0xE000);
    if ((nm_uint16(target_inode.i_mode) & 0xE000) == 0x4000) {
        return true;
    }
    switch (nm_uint16(target_inode.i_mode) & 0xE000) {
    case 0x4000:
        //    kprintf("isdirectory(%lu) == true\r\n", inode);
        return true;
        break;
    case 0x8000:
        //   kprintf("isdirectory(%lu) == false\r\n", inode);
        return false;
        break;
    default:
        kprintf("dunno1!\r\n");
        assert(NULL);
        break;
    }

    kprintf("dunno2!\r\n");
    return false;
}


#define MAX_PATH_ELEMENT_LEN    256

uint32_t ext2_get_cwd_inode()
{
    return ext2_rootfs.cwd_inode;
}

uint32_t ext2_path_to_inode(char *path, uint32_t traverse_inode)
{
    unsigned char path_element[MAX_PATH_ELEMENT_LEN];
    uint16_t path_element_len = 0;
//   uint32_t current_inode = ext2_rootfs.cwd_inode;
    //static uint32_t traverse_inode = 0;
    uint32_t lookup_inode = 0;
    struct ext2_inode recurse_inode;
    struct ext2_inode target_inode;
    char *p = path;
    uint32_t il = 0;

    if (kstrlen(path) == 1 && path[0] == '/') {
        return EXT2_ROOT_INODE;
    }

    if (kstrlen(path) > 1) {
        while (path[kstrlen(path) - 1] == 0x2F) {
            /* chomp trailing backslashes */
            path[kstrlen(path) - 1] = '\0';
        }

        /* collapse leading slashes */
    }

    if (kstrlen(path) > 1) {
        while (path[0] == 0x2F && path[1] == 0x2F && kstrlen(path) >= 2) {
            path++;
        }
    }

//    kprintf("ext2_path_to_inode(%s, '%c', %u)\r\n", path, p[0], traverse_inode);

//		traverse_inode = ext2_rootfs.cwd_inode;

    if (!ext2_rootfs.active ) {
        set_errno(EIO);
        return 0;
    }


    if (p[0] == '/') {
        /* if leading slash specified, path is absolute, so switch to EXT2_ROOT_INODE before descending */
        //kprintf("[root pivot]\r\n");
        //puts("\r\n");
        traverse_inode = EXT2_ROOT_INODE;
        if (kstrlen(p) == 1) {
            /* just looking for root inode thanks */
            return traverse_inode;
        }
        p++;
    }
    // kprintf("[");

    kernel_memset(&path_element, 0, MAX_PATH_ELEMENT_LEN);
    while (p[0] != '\0' && p[0] != '/' && path_element_len < MAX_PATH_ELEMENT_LEN) {
        //      kprintf("%c", p[0]);
        path_element[path_element_len] = p[0];
        p++;
        path_element_len ++;
    }
//   kernel_puts("]\r\n");

    if (traverse_inode == 0) {
        return 0;
    }

    il = ext2_inode_lookup(traverse_inode, &recurse_inode, false);
//		kprintf("(il = %lu)\r\n", il);

//    assert(ext2_inode_lookup(current_inode, &recurse_inode, false));
//		assert(il);


    switch ((nm_uint16(recurse_inode.i_mode) & 0xE000)) {
    case 0x4000:
        /* directory - open and search it for name */
        //      kprintf("inode %lu: it's a directory!\r\n", current_inode);
        lookup_inode = ext2_get_inode_from_dirent(traverse_inode, (char *) &path_element);
        if (!lookup_inode) {
            set_errno(ENOENT);
            //    kprintf("%s: not found\r\n", path_element);

            return 0;
        }
//       kprintf("lookup_inode = %u\r\n", lookup_inode);

        assert(ext2_inode_lookup(lookup_inode, &target_inode, false));

        switch ((nm_uint16(target_inode.i_mode) & 0xE000)) {
        case 0x4000:
            //          kprintf("inode %lu: it's a directory!\r\n", lookup_inode);
            if (kstrlen(p)) {
                //            kprintf("we need to go deeper!\r\n");
                //assert(NULL);
                p++;
                //ext2_rootfs.cwd_inode = lookup_inode;
                traverse_inode = lookup_inode;
                return ext2_path_to_inode(p, traverse_inode);
            }
            /* else, it's a directory, and the one we want too */
            return lookup_inode;
            break;
        case 0x8000:
            //                kprintf("inode %lu: it's a file!\r\n", lookup_inode);
            if (kstrlen(p)) {
                /* wanted to go further, but we must terminate here */
                return lookup_inode;
            }
            /* otherwise, return this inode reference */
            return lookup_inode;
            break;
        default:
            kprintf("inode %lu: it has mode 0x%04x\n", lookup_inode, (nm_uint16(target_inode.i_mode) & 0xE000));
            assert(NULL);
            break;
        }
        return ((uint32_t) NULL);
        break;
    case 0x8000:
        /* it's a regular file */
//        kprintf("inode %lu: it's a regular file!\r\n", current_inode);
        assert(NULL);
        break;
    default:
        /* it's something else! */
        kprintf("inode %lu: it has mode 0x%04x\n", traverse_inode, (nm_uint16(recurse_inode.i_mode) & 0xE000));
        assert(NULL);
        break;
    }

    return 0;

}


uint32_t ext2_get_inode_block(ext2_inode *e2i, uint32_t file_block_id)
{

    unsigned char indirect_cache[1024];
    unsigned char double_indirect_cache[1024];
    uint32_t *icp = NULL;
    uint32_t *dcp = NULL;

    if (file_block_id >=0 && file_block_id < EXT2_NDIR_BLOCKS) {
        /* DIRECT BLOCK */
        //kprintf("-- inode->block_id[%lu] = 0x%08lx [*DIRECT_BLOCK]\r\n", file_block_id, nm_uint32(e2i->i_block[file_block_id]));
        return nm_uint32(e2i->i_block[file_block_id]);
    }

    assert(!(file_block_id < 0 || file_block_id >= ((EXT2_NDIR_BLOCKS + ext2_rootfs.block_size / sizeof(uint32_t)) + ((EXT2_NDIR_BLOCKS^2) * (ext2_rootfs.block_size / sizeof(uint32_t))))));

    /*
    kprintf("/ INDIRECT BLOCK id=%lu (%lu) indirect_block_index = 0x%lx, address = 0x%08lx \r\n",
                    file_block_id, file_block_id - EXT2_NDIR_BLOCKS,
                    nm_uint32(e2i->i_block[EXT2_NDIR_BLOCKS]),
                    nm_uint32(e2i->i_block[EXT2_NDIR_BLOCKS]) * ext2_rootfs.block_size);
        */

    if (file_block_id >= (EXT2_NDIR_BLOCKS + (ext2_rootfs.block_size / sizeof(uint32_t)))) {
        uint32_t block_deref = 0;
        uint32_t block_page =  0;
        uint32_t block_page_index = 0;

        block_deref = file_block_id - (EXT2_IND_BLOCKS + EXT2_NDIR_BLOCKS);
        block_page = block_deref / (ext2_rootfs.block_size / sizeof(uint32_t));
        block_page_index = block_deref % (ext2_rootfs.block_size / sizeof(uint32_t));
        //kprintf("+ double indirect block, block_id=%lu(0x%08lx) -> %u (%u, %u)\r\n", file_block_id, file_block_id, block_deref, block_page, block_page_index);
        devices[ext2_rootfs.device_number].seek(&devices[ext2_rootfs.device_number],  nm_uint32(e2i->i_block[EXT2_DIND_BLOCK]) * ext2_rootfs.block_size);
        devices[ext2_rootfs.device_number].read(&devices[ext2_rootfs.device_number], (unsigned char *) &double_indirect_cache, ext2_rootfs.block_size);
        dcp = (uint32_t *) &double_indirect_cache;
        //ptr_dump(&double_indirect_cache);
        dcp += block_page;
        //kprintf("dcp = %08lx\r\n", nm_uint32(*dcp));
        /* we have deferenced the double indirect block here, and block_page_index should be an offset into the next page we fetch, which is indirect */
        devices[ext2_rootfs.device_number].seek(&devices[ext2_rootfs.device_number],  nm_uint32((*dcp)) * ext2_rootfs.block_size);
        devices[ext2_rootfs.device_number].read(&devices[ext2_rootfs.device_number], (unsigned char *) &indirect_cache, ext2_rootfs.block_size);
        //ptr_dump(&indirect_cache);
        /* dereference the indirect block */
        icp = (uint32_t *) &indirect_cache;
        icp += block_page_index;
        //kprintf("icp = %08lx\r\n", nm_uint32(*icp));
        return nm_uint32(*icp);
        //assert(NULL);
    } else {
        devices[ext2_rootfs.device_number].seek(&devices[ext2_rootfs.device_number],  nm_uint32(e2i->i_block[EXT2_IND_BLOCK]) * ext2_rootfs.block_size);
        devices[ext2_rootfs.device_number].read(&devices[ext2_rootfs.device_number], (unsigned char *) &indirect_cache, ext2_rootfs.block_size);
        icp = (uint32_t *) &indirect_cache;
        //kprintf("icp[1] = 0x%08lx\r\n", icp);
        icp+= file_block_id - EXT2_NDIR_BLOCKS;
        //kprintf("icp[2] = 0x%08lx\r\n", icp);
        //kprintf("indirect_block_pointer = %08lx\r\n", nm_uint32(*icp));
        //ptr_dump(&indirect_cache);
        return nm_uint32(*icp);
    }

    /* TODO: triple indirect blocks */

    assert((file_block_id < (EXT2_NDIR_BLOCKS)));
    return 0;
}


uint32_t ext2_block_read(ext2_fs *fs, uint32_t dma_addr, uint32_t block_id)
{

    kprintf("ext2_block_read([%u], 0x%08lx, 0x%08lx)\r\n", fs->device_number, dma_addr, block_id);
    devices[fs->device_number].seek(&devices[fs->device_number], block_id * fs->block_size);
    devices[fs->device_number].read(&devices[fs->device_number], (unsigned char *) dma_addr, fs->block_size);
    return 1;
}

uint32_t ext2_next_free_inode(ext2_fs *fs)
{
    unsigned char block_bitmap[fs->block_size];
    uint32_t bitmap_index = 0;
    uint16_t bitmap_byte = 0;
    uint8_t byte_offset = 128;

    kprintf("ext2_next_free_inode(%s)\n\r", fs->device_number);
    devices[fs->device_number].seek(&devices[fs->device_number], ext2_rootfs.inode_bitmap);
    devices[fs->device_number].read(&devices[fs->device_number], (unsigned char *) &block_bitmap, fs->block_size);

    if (!devices[fs->device_number].write) {
        kprintf("FATAL: device is not writable!\n\r");
        set_errno(EROFS);
        return 0;
    }

    bitmap_byte = bitmap_index / 8;
    byte_offset = 128 >> (bitmap_index % 8);

    while (bitmap_index < 2048) {
        bitmap_byte = bitmap_index / 8;
        byte_offset = 128 >> (bitmap_index % 8);
        switch (block_bitmap[bitmap_byte] & byte_offset) {
        case 0x00:
            if (bitmap_index < nm_uint32(ext2_rootfs.blck.s_first_ino)) {
                kprintf("inode %04u: [%03u:%03u] RESV\n\r", bitmap_index+1, bitmap_byte, byte_offset);
                bitmap_index ++;

            } else {
                kprintf("inode %04u: [%03u:%03u] FREE\n\r", bitmap_index+1, bitmap_byte, byte_offset);

                kprintf("was: %02x\n", block_bitmap[bitmap_byte]);
                block_bitmap[bitmap_byte] |= byte_offset;
                kprintf("now: %02x\n", block_bitmap[bitmap_byte]);

                /* write the modified bitmap block */
                devices[fs->device_number].seek(&devices[fs->device_number], ext2_rootfs.inode_bitmap);
                devices[fs->device_number].write(&devices[fs->device_number], (unsigned char *) &block_bitmap, fs->block_size);

                return bitmap_index+1;
            }
            break;
        case 0x01:
            kprintf("inode %04u: [%03u:%03u] USED\n\r", bitmap_index+1, bitmap_byte, byte_offset);
            break;
        }
    }

    return 0;
}
