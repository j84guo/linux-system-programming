#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define RECVSIZE 2048

/** IP agnostic server socket */
struct ServerSock
{
    /** socket descriptor */
    int sd;

    /** socket address */
    struct sockaddr_storage addr;

    /** address family */
    int family;
};

/** initialize socket and address */
int udp_serv(struct ServerSock *serv, char *port);

/** extract positive integer from string */
int parse_port(char *str);

/** addrinfo constraints */
void udp_serv_hints(struct addrinfo *hints);

/** close file descriptor and log to stderr on failure */
void try_close(int fd);

/** try to initialize serv with the first working addrinfo struct */
int init_with_first(struct ServerSock *serv, struct addrinfo *res);

/** print address info structure */
void print_addrinfo(struct addrinfo *ptr);

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return 1;
    }

    struct ServerSock serv;
    if(!udp_serv(&serv, argv[1]))
    {
        fprintf(stderr, "udp_serv: failed\n");
        return 1;
    }

    while(1)
    {
        struct sockaddr_storage peer;
        socklen_t len = sizeof peer;

        char buf[RECVSIZE + 1];
        int ret = recvfrom(serv.sd, buf, sizeof buf, 0, (struct sockaddr *) &peer, &len);
        if(ret == -1)
        {
            perror("recvfrom");
            continue;
        }

        buf[ret] = '\0';
        printf("received: %s", buf);

        print_sockadd(&peer);
    }
}

int udp_serv(struct ServerSock *serv, char *port)
{
    if(serv == NULL)
        return 0;

    struct addrinfo hints;
    udp_serv_hints(&hints);

    int ret;
    struct addrinfo *res;
    if((ret = getaddrinfo(NULL, port, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return 0;
    }

    ret = init_with_first(serv, res);
    freeaddrinfo(res);

    return !ret ? 0 : 1;
}

void udp_serv_hints(struct addrinfo *hints)
{
    if(hints == NULL)
        return;

    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_DGRAM;
    hints->ai_flags = AI_PASSIVE;
}

int init_with_first(struct ServerSock *serv, struct addrinfo *res)
{
    if(serv == NULL)
        return 0;

    for( ; res != NULL; res = res->ai_next)
    {
        print_addrinfo(res);

        int sd;
        if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
        {
            perror("close");
            continue;
        }
        serv->sd = sd;
        
        if(bind(serv->sd, res->ai_addr, res->ai_addrlen) == -1)
        {
            perror("bind");
            try_close(serv->sd);
            continue;
        }

        serv->family = res->ai_family;
        memcpy(&serv->addr, &res->ai_addr, res->ai_addrlen);        

        printf("success\n");
        return 1;
    }

    return 0;
}

void try_close(int fd)
{
    if(close(fd) == -1)
        perror("close");
}

void print_addrinfo(struct addrinfo *ptr)
{
    if(ptr == NULL)
        return;
    
    void *addr;
    unsigned short port;

    if(ptr->ai_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) ptr->ai_addr;
        addr = &ipv4->sin_addr;
        port = ntohs(ipv4->sin_port);
    }
    else
    {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
        addr = &ipv6->sin6_addr;
        port = ntohs(ipv6->sin6_port);
    }

    char buf[INET6_ADDRSTRLEN];
    inet_ntop(ptr->ai_family, addr, buf, sizeof buf);

    printf("%s:%hu\n", buf, port);
}

/*
void print_sockaddr()
{

}
*/
