#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    // if(argc != 2)
    //     return 1;
    //
    // char *end;
    // long l = strtol(argv[1], &end, 10);
    //
    // printf("%ld\n%s\n", l, end);

    int a = 0;
    int b = 12;

    memcpy(&a, &b, 4);
    printf("%d %d\n", a, b);

    struct sockaddr_storage stor;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_storage *dst = &stor;
    struct sockaddr_in *src = &addr;

    memcpy(dst, src, sizeof(struct sockaddr_in));
    printf("%d\n", stor.ss_family);
}
