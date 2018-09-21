#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 2

int main(int argc, char **argv)
{
    int pd[2];
    if(pipe(pd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return 1;
    }

    if(pid == 0) {
        if(close(pd[1]) == -1) {
            perror("close\n");
            return 1;
        }

        int cap=BUFSIZE, n=0, m;
        char *buf = malloc(cap + 1);
        
        while(m = read(pd[0], buf + n, BUFSIZE)) {
            if(m == -1) {
                perror("read");
                return 1;
            }

            n += m;

            if(n > cap - BUFSIZE) {
                cap *= 2;
                buf = realloc(buf, cap + 1);
            }
        }
    
        buf[n] = 0;
        printf("n: %d\n", n);
        printf("buffer: %s\n", buf);
        printf("pipe descriptors: %d, %d\n", pd[0], pd[1]);

        free(buf);
        return 0;
    }

    printf("forked child %d\n", pid);
    char buf[] = "hello child process, here's some data through the pipe";
    size_t cap = strlen(buf);
    int n = 0;
    
    while(n < cap) {
        int m = write(pd[1], buf + n, cap - n);

        if(m == -1) {
            if(errno == EINTR)
                continue;

            perror("write");
            return 1;
        }

        printf("wrote %d bytes\n", m);
        n += m;
    }
        
    if(close(pd[0]) == -1 || close(pd[1]) == -1) {
        perror("close");
        return 1;
    }
}
