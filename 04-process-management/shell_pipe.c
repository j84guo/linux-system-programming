#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Descriptors
{
    int pd;
    int sd;
    int bd;
};

void dup2_try(int oldfd, int newfd)
{
    if(dup2(oldfd, newfd) == -1) {
        perror("dup2");
        exit(1);
    }
}

void fork_exec(const char *file, char *const argv[])
{
    pid_t pid = fork();

    if(pid == -1) {
        perror("fork");
        exit(1);
    } else if(pid == 0) {
        execvp(file, argv);
        perror("execvp");
        exit(1);
    }
}

void wait_try()
{
    int status;
    if(wait(&status) == -1) {
        perror("wait");
        exit(1);
    }
}

void cmd_try(const char * file, char * const argv[], struct Descriptors des)
{
    dup2_try(des.pd, des.sd);
    fork_exec(file, argv);
    wait_try();
    dup2_try(des.bd, des.sd);

    if(close(des.pd) == -1) {
        perror("close");
        exit(1);
    }
}

int main()
{
    int in_d, out_d;
    if((in_d = dup(0)) == -1 || (out_d = dup(1)) == -1) {
        perror("dup");
        return 1;
    }

    int pd[2];
    if(pipe(pd) == -1) {
        perror("pipe");
        return 1;
    }
 
    const char *file = "ls";
    char * const argv1[] = {"ls", "-la", NULL};
    struct Descriptors des = {pd[1], 1, out_d};
    cmd_try(file, argv1, des);

    file = "cat";
    char * const argv2[] = {"cat", NULL};
    des.pd = pd[0];
    des.sd = 0;
    des.bd = in_d;
    cmd_try(file, argv2, des);
}
