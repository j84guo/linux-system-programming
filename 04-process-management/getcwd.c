#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFINIT 2

int main()
{
    int cap = BUFINIT;
    char *buf = malloc(cap);

    while(getcwd(buf, cap) == NULL) {
        if(errno != ERANGE) {
            perror("getcwd");
            NULL;
        }

        cap *= 2;
        buf = realloc(buf, cap);
    }

    printf("cwd: %s\n", buf);
}
