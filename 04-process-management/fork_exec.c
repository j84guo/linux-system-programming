#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int redirect_outerr()
{
    close(1);
    close(2);

    if(open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644) == -1)
        return -1;

    dup(1);
    return 0;
}

void wait_print(pid_t pid)
{
    int status;

    if(waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        return;       
    }

    if(WIFEXITED(status))
        printf("command finished with: %d\n", WEXITSTATUS(status));

    if(WIFSIGNALED(status))
        printf("killed by signal: %d\n", WTERMSIG(status));

    if(WIFSTOPPED(status))
        printf("stopped by signal: %d\n", WSTOPSIG(status));

    if(WIFCONTINUED(status))
        printf("continued");
}

int main()
{
    if(redirect_outerr() == -1)
    {
        perror("redirect_outerr");
        return 1;
    }

    /*
     * e.g. spawn a child and run /bin/ls
     */                   
    pid_t pid = fork();
    
    if(pid == -1)
    {
        perror("fork");
    }
    else if(pid == 0)
    {
        char *const args[] = {"ls", NULL};
        int ret = execv("/bin/ls", args);
        if(ret == -1)
        {
            perror("execv");
            exit(1);
        }
    }

    wait_print(pid);
    return 0;
}
