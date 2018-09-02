/**
 * IPv4 TCP client which reads and writes with a host specified by address and
 * port. The snippets here are re-usable and intended to simplify working with
 * the Unix socket API.
 */

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/** Receive buffer */
#define RECVSIZE 512

/**
 * A struct representing an IPv4 TCP client, includes the socket descriptor,
 * struct sockaddr_in, string form ip and numeric port
 */
struct ClientSock
{
    /** socket descriptor */
    int sd;

    /** ip in string form */
    char ip[INET6_ADDRSTRLEN];

    /** port **/
    unsigned short port;

    /** address and port */
    struct sockaddr_in addr;
};

/**
 * Parse string into binary ip, fill struct sockaddr_in, acquire socket
 * descriptor
 */
int client_socket(struct ClientSock *client, char *ip, unsigned short port);

/**
 * Connect to host
 */
int client_connect(struct ClientSock *client);

/**
 * Send len bytes from buf to the socket descriptor contained in client
 */
int client_sendall(struct ClientSock *client, void *buf, size_t len);

/**
 * Wrapper around normal recv()
 */
int client_recv(struct ClientSock *client, void *buf, size_t len);

/**
 * Release system socket
 */
int client_close(struct ClientSock *client);

int main(int argc, char **argv)
{
    struct ClientSock client;

    if(!client_socket(&client, "127.0.0.1", 8888))
    {
        fprintf(stderr, "client_socket: failed\n");
        return 1;
    }
    printf("created client to %s %hu\n", client.ip, client.port);

    if(!client_connect(&client))
    {
        fprintf(stderr, "client_connect: failed\n");
        return 1;
    }
    printf("connected\n");

    char *send_buf = "this is a message sent over TCP\n";
    if(client_sendall(&client, send_buf, strlen(send_buf)) == -1)
    {
        perror("client_sendall");
        return 1;
    }

    char recv_buf[RECVSIZE + 1];
    int ret;
    if((ret = client_recv(&client, recv_buf, RECVSIZE)) == -1)
    {
        perror("client_recv");
        return 1;
    }else
        recv_buf[ret] = '\0';

    printf("%s", recv_buf);

    if(client_close(&client) == -1)
    {
        perror("client_close");
        return 1;
    }
}

int client_socket(struct ClientSock *client, char *ip, unsigned short port)
{
    if(client == NULL)
        return 0;

    /**
     * Convert the string to an address first, as nothing else is necessary
     * if it is invalid
     */ 
    if(inet_pton(AF_INET, ip, &client->addr.sin_addr) != 1)
        return 0;
    else
    {
        strncpy(client->ip, ip, INET6_ADDRSTRLEN);
        client->port = port;
    }

    /**
     * Address family is hardcoded, port should be valid due to static type
     */
    client->addr.sin_family = AF_INET;
    client->addr.sin_port = htons(port);

    /**
     * Zero-ing sin_zero may be optional
     */ 
    memset(&client->addr.sin_zero, 0, sizeof client->addr.sin_zero);

    /**
     * Socket descriptor
     */ 
    if((client->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        return 0;

    return 1;
}

int client_connect(struct ClientSock *client)
{
    if(connect(client->sd, (struct sockaddr *) &client->addr, sizeof(struct sockaddr_in)) == -1)
        return 0;
    else
        return 1;   
}

int client_sendall(struct ClientSock *client, void *buf, size_t len)
{
    int n = 0;
    int ret;    

    while(n < len)
    {
        ret = send(client->sd, buf, len, 0);

        if(ret == -1)
            return -1;

        n += ret;
    }

    return n;
}

int client_recv(struct ClientSock *client, void *buf, size_t len)
{
    return recv(client->sd, buf, len, 0);
}

int client_close(struct ClientSock *client)
{
    return close(client->sd);
}
