#ifndef __FCNTL_PRIVATE_H__
#define __FCNTL_PRIVATE_H__

#include "fcntl.h"

#define FILES_MAX   4
#define SSIZE_MAX 128

extern FILE filehandles[FILES_MAX];
extern _cfd CFD[FILES_MAX];
extern bool _fds_init_done;
extern int  _find_free_fd();
extern int  _find_free_filehandle();
extern void _fds_init();
//extern char * _print_fcb(FCB *fcb_ptr, bool brief);
extern uint8_t dma_buffer[SSIZE_MAX];

#endif
