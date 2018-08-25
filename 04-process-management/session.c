#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid, sid;

    if((pid = fork()) == -1)
    {
        perror("fork");
        return -1;
    }
    else if(pid != 0)
    {
        printf("parent: %d\n", getpid());
        exit(0);
    }

    printf("child: %d\n", getpid());
 
    if(setsid() == -1)
    {
        perror("setsid");
        return -1;
    }

    sid = getsid(0);
    printf("session: %d\n", sid);
}
