#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * synchronously spawns and waits for the command /bin/sh -c <cmd>
 * returns -1 for any error, otherwise the exit code of the command
 * does not ignore signals
 */
int my_system(const char *cmd)
{
    int status;

    pid_t pid = fork();
    
    if(pid == -1)
    {
        return -1;
    }
    else if(pid == 0)
    {
        const char *argv[4];
        argv[0] = "sh";
        argv[1] = "-c";
        argv[2] = cmd;
        argv[3] = NULL;

        execv("/bin/sh", argv);
        
        // if execv returns, it is -1 and indicates an error
        exit(-1);
    }

    if(waitpid(pid, &status, 0) == -1)
        return -1;
    else if(WIFEXITED(status))
        return WEXITSTATUS(status);
    
    // -1 is returned if the process is stopped by a signal
    return -1;
}

int main()
{
    printf("%d\n", my_system("git status"));    
}
