#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
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
}
