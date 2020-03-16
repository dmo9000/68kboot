#ifndef __KERNEL_H__
#define __KERNEL_H__

int kprintf(const char* format, ...);
void *kernel_memcpy(void *dest, const void *src, size_t len);
int kernel_putchar(int c);
void *kernel_memset(void *s, int c, size_t n);
char * kernel_strncat(char *s1, char *s2, int n);
int kernel_strncmp(const char *s1, const char *s2, size_t n);
int kernel_puts(const char *s);



#endif /* __KERNEL_H__ */
