#define NULL 0


int pad(int c, int t)
{
    int i = 0;

//	printf ("<pad %u, %u>", c, t);
//	return 0;

    for (i = 0; i<c; i++) {
        switch (t) {
        case 1:
            putchar('0');
            break;
        case 2:
            putchar(' ');
            break;
        }
    }
}

printf (const char *format, ...)
{
    char **arg = (char **) &format;
    int c;
    char buf[20];
    char mod[20];
    int modptr = 0;
    int padding = 0;
    arg++;

    memset(&mod, 0, 20);

    while ((c = *format++) != 0)
    {
        if (c != '%')
            putchar (c);
        else
        {
            char *p;

            c = *format++;

            /* support padding - DM */

            while (isdigit(c)) {
                mod[modptr] = c;
                c = *format++;
                modptr++;
            }

            if (modptr > 0) {
                if (mod[0] == '0') {
                    /* set 0 padding flag */
                    padding = 1;
                } else {
                    padding = 2;
                }
            }

            switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
                itoa (buf, c, *((int *) arg++));
                if (padding) {
                    //printf("<P%s,%u,%u>", mod, strtoul(mod, NULL, 10), strlen(buf));
                    pad(strtoul(mod, NULL, 10) - strlen(buf), padding);
                }
                p = buf;
                goto string;
                break;

            case 's':
                p = *arg++;
                if (!p)
                    p = "(null)";

string:
                while (*p)
                    putchar (*p++);
                break;

            default:
                putchar (*((int *) arg++));
                break;
            }
            memset(&mod, 0, 20);
            modptr = 0;
            padding = 0;
        }
    }
}
