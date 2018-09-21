#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <fifo name>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) {
        perror("open");
        return 1;
    }

    int nr;
    char buf[64];
    while((nr = read(fd, buf, sizeof buf)) != 0) {
        if(nr == -1) {
            if(errno == EINTR)
                continue;

            perror("read");
            return 1;
        }

        buf[nr] = 0;
        printf("%s", buf);
    }
    
}
