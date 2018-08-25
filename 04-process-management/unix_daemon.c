#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    if(daemon(0, 1) == -1)
    {
        printf("error");
        return 1;
    }
    
    int fd = open("daemon.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd == -1)
    {
        perror("open");
        return 1;
    }    
    
    char* buf = "this is some daemon output";

    ssize_t n = 0;
    ssize_t m = 0;

    while(n < strlen(buf))
    {
        m = write(fd, buf + n, strlen(buf));

        if(m == -1)
        {
            if(errno == EINTR)
                continue;
            
            perror("write");
            return 1;
        }

        n += m;
    }

    if(close(fd) == -1)
    {
        perror("close");
        return 1;
    }
}
