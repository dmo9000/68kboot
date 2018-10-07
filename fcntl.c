#include "stdio.h"
#include "fcntl.h"

_fd file_descriptor[MAX_FDS];

bool file_table_initialized = false;


void initialize_file_table()
{
    int i = 0;

    for (i = 0; i < MAX_FDS; i++) {
        file_descriptor[i].state = FD_STATE_UNUSED; 
        }

    file_descriptor[0].state = FD_STATE_STDIN;
    file_descriptor[1].state = FD_STATE_STDOUT;
    file_descriptor[2].state = FD_STATE_STDERR;


    file_table_initialized = true;
    return;
}

int fcntl_find_free_fd()
{

    int i = 0;

    if (!file_table_initialized) {
            initialize_file_table();
            }

    for (i = 0; i < MAX_FDS; i++) {
        if (file_descriptor[i].state  == FD_STATE_UNUSED) {
            return i;
            }
        }
    return -1;

}

int fcntl_open_inode(uint32_t inode, int flags) 
{
    int new_fd = -1;
    new_fd = fcntl_find_free_fd();
    file_descriptor[new_fd].state = FD_STATE_OPEN;
    file_descriptor[new_fd].inode = inode;
    /* TODO: check flags validity here */
    file_descriptor[new_fd].flags = flags;
    /* position at start of file */
    file_descriptor[new_fd].offset = 0;
    return new_fd;

}
