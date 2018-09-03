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

/** IP version agnostic socket client */
struct ClientSock
{
    /** socket descriptor */
    int sd;

    /** either struct sockaddr_in or struct sockaddr_in6 */
    struct sockaddr_storage addr;
};

/** init udp client struct */
int init_udp_client(struct ClientSock *client, char *host, char *port);

/** init hints struct for getaddrinfo */
void udp_client_hints(struct addrinfo *hints);

/** use first struct addrinfo to init client */
int init_with_first(struct ClientSock *client, struct addrinfo *res);

/** print address and port */
void print_udp_client(struct ClientSock *client);

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 1;
    }

    struct ClientSock client;
    if(!init_udp_client(&client, argv[1], argv[2]))
    {
        fprintf(stderr, "init_udp_client: failed\n");
        return 1;
    }
    print_udp_client(&client);

    char buf[] = "hello world from a udp client\n";
    int ret = sendto(client->sd, buf, sizeof buf, 0,
        (struct sockaddr *) client->addr, sizeof(struct sockaddr_storage));

    if(ret == -1)
        perror("sendto");
}

int init_udp_client(struct ClientSock *client, char *host, char *port)
{
    struct addrinfo hints;
    udp_client_hints(&hints);

    struct addrinfo *res;
    int ret = getaddrinfo(host, port, &hints, &res);
    if(ret != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return 0;
    }

    ret = init_with_first(client, res);
    freeaddrinfo(res);
    return ret;
}

int init_with_first(struct ClientSock *client, struct addrinfo *res)
{
    for( ; res != NULL; res = res->ai_next)
    {
        int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        if(sd == -1)
        {
            perror("socket");
            continue;
        }

        client->sd = sd;
        memcpy(&client->addr, res->ai_addr, res->ai_addrlen);
        return 1;
    }

    return 0;
}

void udp_client_hints(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(struct addrinfo));

    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_DGRAM;
    hints->ai_protocol = IPPROTO_UDP;
}

void print_udp_client(struct ClientSock *client)
{
    void *ia;
    unsigned short port;

    if(client->addr.ss_family == AF_INET)
    {
        struct sockaddr_in *sa = (struct sockaddr_in *) &client->addr;
        ia = &sa->sin_addr;
        port = ntohs(sa->sin_port);
    }
    else
    {
        struct sockaddr_in6 *sa = (struct sockaddr_in6 *) &client->addr;
        ia = &sa->sin6_addr;
        port = sa->sin6_port;
    }

    char buf[INET6_ADDRSTRLEN];
    const char *ret = inet_ntop(client->addr.ss_family, ia, buf, sizeof buf);

    if(!ret)
        perror("inet_ntop");
    else
        printf("ClientSock: [%s:%hu]\n", buf, port);
}
