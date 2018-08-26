#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int do_cmd(char **argv)
{
    execvp(argv[1], &argv[1]);

    perror("execvp");
    return 1;
}

/*
 * fork a child and execvp the whitespace separated arguments
 *
 * note if the child is stopped (SIGSTOP) and the parent terminate, the entire
 * process group is sent SIGHUP + SIGCONT, the default behaviour of SIGHUP will
 * cause the members to terminate: https://stackoverflow.com/questions/4354885/
 * child-processes-die-when-killing-parent-if-one-of-them-is-stopped-with-
 * sigstop
 */
int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "usage: %s <cmd> <args ...>\n", argv[0]);
        exit(1);
    } 

    pid_t pid = fork();
    
    if(pid == -1)
    {
        perror("fork");
        return 1;
    }
    else if(pid == 0)
    {
       return do_cmd(argv);
    }
    else
    {
        int status;

        if(waitpid(pid, &status, WUNTRACED) == -1)
        {
            perror("waitpid");
            return 1;
        }

        if(WIFEXITED(status))
            printf("child %d exited: %d\n", pid, WEXITSTATUS(status));

        if(WIFSIGNALED(status))
            printf("child %d signaled: %d\n", pid, WTERMSIG(status));

        if(WIFSTOPPED(status))
            printf("child %d stopped: %d\n", pid, WSTOPSIG(status));

        if(WIFCONTINUED(status))
            printf("child %d continued\n", pid);
    }

    
    return 0;
}

