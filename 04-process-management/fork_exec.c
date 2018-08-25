#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int redirect_outerr()
{
    close(1);
    close(2);

    if(open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644) == -1)
        return -1;

    dup(1);
    return 0;
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
}
