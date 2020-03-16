#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#define VERSION_MAJOR       0
#define VERSION_MINOR       0
#define VERSION_REVISION   	68

typedef struct _bdos_vtable {
    uint32_t magic;
    uint8_t ver_maj;
    uint8_t ver_min;
    uint8_t ver_rev;
    int errno;
    bool initialized;
    uint8_t selected_drive;
    int (*_open)(const char *pathname, int flags);
    off_t (*_lseek)(int fd, off_t offset, int whence);
    ssize_t (*_read)(int fd, void *buf, size_t count);
    ssize_t (*_write)(int fd, void *buf, size_t count);
    int (*_close)(int fildes);
    int (*_stat)(const char *restrict path, struct stat *restrict buf);
    int (*_chdir)(const char *path);
//    char * (*_getcwd)();
    time_t (*_time)(time_t *tloc);
    char *(*_getenv)(const char *name);
    int (*_fcntl)(int fd, int c, int tf);
    int (*_tcgetattr)(int fd, struct termios *termios_p);
    int (*_tcsetattr)(int fd, int optional_actions, const struct termios *termios_p);
		int (*_blkdevseek)(uint8_t dev, off_t offset);
		int (*_blkdevread)(uint8_t dev, char *buffer, size_t len);
		int (*_blkdevwrite)(uint8_t dev, char *buffer, size_t len);

} _bdos_vtable;

int bdos_init();

