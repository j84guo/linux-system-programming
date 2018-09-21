#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    dup2_try(pd[1], 1);
    fork_exec(file, argv1);
    dup2_try(out_d, 1);

    if(close(pd[1]) == -1) {
        perror("close");
        return 1;
    }

    file = "ls";
    char * const argv2[] = {"cat", "cat", NULL};
    dup2_try(pd[0], 0);
    fork_exec(file, argv1);
    dup2_try(in_d, 0);
}
