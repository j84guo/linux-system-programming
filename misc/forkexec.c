#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int spawn(char *program, char **arg_list)
{
    pid_t child = fork();

    if (!child) {
        execvp(program, arg_list);
        perror("execvp");
        abort();
    } else if (child == -1) {
        perror("fork");
    }

    return child;
}

int main()
{
    char *arg_list[] = {
        "ls",
        "-la",
        "/",
        NULL
    };
    
    if (spawn("ls", arg_list) == -1)
        return 1;

    int status;
    pid_t ret = waitpid(-1, &status, 0);

    if (ret == -1) {
        perror("waitpid");
        return 1;
    }

    printf("waited on child=%d status=%d\n", ret, status);

    return 0;
}
