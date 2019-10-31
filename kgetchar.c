int kernel_getchar()
{
    char *  p = (char *) 0xff1002;
    return p[0];
}

