#define NULL 0

char *kernel_strchr(const char *s, int c)
{
    int l = 0;
    int i = 0;

    l = kstrlen(s);

    for (i = 0; i< l; i++)
    {
        if (s[0] == c) {
            return s;
        }
        s++;
    }
    return NULL;
}
