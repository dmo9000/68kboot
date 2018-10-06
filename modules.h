#include "stdbool.h"

int modules(char *payload);

struct _jump_table {
    int (*cmdptr)(char *payload);
    char *command;
};

typedef struct _jump_table jmpTable;

extern const jmpTable jmptbl[];

