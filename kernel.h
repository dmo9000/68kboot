#ifndef __KERNEL_H__
#define __KERNEL_H__

int kernel_printf(const char* format, ...);
void *kernel_memcpy(void *dest, const void *src, size_t len);
int kernel_putchar(int c);
void *kernel_memset(void *s, int c, size_t n);
char * kernel_strncat(char *s1, char *s2, int n);
int kernel_stat(const char *restrict path, struct stat *restrict buf);

#endif /* __KERNEL_H__ */
