/*
 * e.g. wait() on a child
 */ 

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int status;
    pid_t pid;

    if(!fork())
        return 1;

    pid = wait(&status);
    if(pid == -1)
        perror("wait");

    printf("pid=%d\n", pid);

    if(WIFEXITED(status))
        printf("exit status: %d\n", WEXITSTATUS(status));

    if(WIFSIGNALED(status))
        printf("killed by signal: %d\n", WTERMSIG(status));

    if(WIFSTOPPED(status))
        printf("stopped by signal: %d\n", WSTOPSIG(status));

    if(WIFCONTINUED(status))
        printf("continued\n");

    return 0;
}

