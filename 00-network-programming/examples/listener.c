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
#define BUFINIT 8

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

/** addrinfo constraints */
void udp_serv_hints(struct addrinfo *hints);

/** close file descriptor and log to stderr on failure */
void try_close(int fd);

/** try to initialize serv with the first working addrinfo struct */
int init_with_first(struct ServerSock *serv, struct addrinfo *res);

/** print address info structure */
void print_addrinfo(struct addrinfo *ptr);

/** receive messages indefinitely */
void serve_forever(struct ServerSock *serv);

/** handle one message */
void handle_message(struct ServerSock *serv);

/** read stdin for a message and send it back to the peer */
void reply_message(struct ServerSock *serv, struct sockaddr_storage *peer);

/** read one line from stdin */
char *input_reply();

/** prints contents of a sockaddr_storage based on address family */
void print_sockaddr(struct sockaddr_storage *peer);

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

    serve_forever(&serv);
}

void serve_forever(struct ServerSock *serv)
{
    while(1)
    {
        handle_message(serv);
    }
}

void handle_message(struct ServerSock *serv)
{
    struct sockaddr_storage peer;
    socklen_t len = sizeof peer;

    char buf[RECVSIZE + 1];
    int ret = recvfrom(serv->sd, buf, sizeof buf, 0, (struct sockaddr *) &peer, &len);

    if(ret == -1)
    {
        perror("recvfrom");
        return;
    }
    else
        buf[ret] = '\0';

    printf("received: %s", buf);
    print_sockaddr(&peer);

    reply_message(serv, &peer);
}

void reply_message(struct ServerSock *serv, struct sockaddr_storage *peer)
{
    char *buf = input_reply();
    socklen_t len = peer->ss_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

    if(sendto(serv->sd, buf, strlen(buf), 0, (struct sockaddr *) peer, len) == -1)
    {
        perror("sendto");
    }

    free(buf);
}

char *input_reply()
{
    printf("reply: ");

    int c, n = 0, cap = BUFINIT;
    char *buf = (char *) malloc(sizeof(char) * cap);

    while((c = fgetc(stdin)) != EOF)
    {
        if(n == cap)
        {
            cap *= 2;
            buf = (char *) realloc(buf, sizeof(char) * cap);
        }

        buf[n++] = (char) c;
        
        if(c == '\n')
            break;
    }

    if(ferror(stdin))
    {
        perror("fgetc");
        exit(1);
    }
    else if(feof(stdin))
        exit(0);

    buf[n] = '\0';
    return buf;
}

void print_sockaddr(struct sockaddr_storage *peer)
{
    if(peer == NULL)
        return;

    void *addr;
    unsigned short port;

    if(peer->ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) peer;
        addr = &ipv4->sin_addr;
        port = ntohs(ipv4->sin_port);
    }
    else
    {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) peer;
        addr = &ipv6->sin6_addr;
        port = ntohs(ipv6->sin6_port);
    }

    char buf[INET6_ADDRSTRLEN];
    inet_ntop(peer->ss_family, addr, buf, sizeof buf);
    printf("peer [%s:%hu]\n", buf, port);
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

        printf("init_with_first: success\n");
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
