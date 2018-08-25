/*
 * e.g. wait() on a child
 */ 

#include <unistd.h> // fork
#include <stdio.h> // printf, perror
#include <stdlib.h> // abort
#include <sys/types.h> // pid_
#include <sys/wait.h> // wait, W* macros

int main()
{
    int status;
    pid_t pid;

    if(fork() == 0)
        // signal 6
        // abort();
        return 0;

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

