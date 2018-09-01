#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define RECVSIZE 512

void init_hints(struct addrinfo *hints);
int request(struct addrinfo *ptr);
void print_server(struct addrinfo *ptr);
void *get_address(struct addrinfo *ptr);

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    init_hints(&hints);

    int ret;
    struct addrinfo *res;
    if((ret = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return 1;
    }

    struct addrinfo *ptr;
    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        if(!request(ptr))
            break;
    }

    if(ptr == NULL)
    {
        fprintf(stderr, "request: Failed\n");
        return 1;
    }

    return 0;
}

void init_hints(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(struct addrinfo));

    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;
}

int request(struct addrinfo *ptr)
{
    int sd;
    if((sd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1)
    {
        perror("socket");
        return -1;
    }

    if(connect(sd, ptr->ai_addr, ptr->ai_addrlen) == -1)
    {
        if(close(sd) == -1)
            perror("close");

        perror("connect");
        return -1;
    }

    print_server(ptr);

    int ret;
    char buf[RECVSIZE + 1];
    if((ret = recv(sd, buf, RECVSIZE, 0)) == -1)
    {
        perror("recv");
        return -1;
    }
    else if(ret == 0)
    {
        fprintf(stderr, "request: server disconnected");
        return -1;
    }

    buf[ret] = '\0';
    printf("%s", buf); 
    return 0;
}

void print_server(struct addrinfo *ptr)
{
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(ptr->ai_family, get_address(ptr), ip, sizeof ip);
    printf("connected with: %s\n", ip);
}

void *get_address(struct addrinfo *ptr)
{
    if(ptr->ai_family == AF_INET)
    {
        return &((struct sockaddr_in *) ptr->ai_addr)->sin_addr;
    }
    else
    {
        return &((struct sockaddr_in6 *) ptr->ai_addr)->sin6_addr;
    }
}
