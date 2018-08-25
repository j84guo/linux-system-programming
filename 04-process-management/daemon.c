#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

void daemonWork()
{
    printf("hello world\n");
}

int main()
{
    pid_t pid = fork();
    
    if(pid == -1)
    {
        perror("fork");
        return -1;
    }
    else if(pid != 0)
    {
        printf("parent: %d exiting\n", getpid());
        exit(0);
    }

    printf("child: %d started\n", getpid());

    if(setsid() == -1)
    {
        perror("setsid");
        return 1;
    }

    if(chdir("/") == -1)
    {
        perror("chdir");
        return 1;
    }

    // close open file descriptors
    
    // open redirect stdin, stdout, stderr
    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
    
    daemonWork();

    return 0;
}
