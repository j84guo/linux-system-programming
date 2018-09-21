#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define BUFINIT 128;

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <fifo name>\n", argv[0]);
        return 1;
    }

    int cap = BUFINIT;
    char *cwd = malloc(cap);
    
    while(getcwd(cwd, cap) == NULL) {
        if(errno != ERANGE) {
            perror("getcwd");
            return 1;
        }

        cap *= 2;
        cwd = realloc(cwd, cap);
    }

    char *name = argv[1];        
    char *abs = malloc(strlen(cwd) + strlen(name) + 2);    
    strcat(abs, cwd);
    strcat(abs, "/");
    strcat(abs + strlen(cwd), name);   
    printf("creating fifo: %s\n", abs);

    if(mkfifo(abs, 0664) == -1) {
        perror("mkfifo");
        return 1;
    }

    free(cwd);
    free(abs);
}
