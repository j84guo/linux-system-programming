#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h> // getaddrinfo, addrinfo
#include <arpa/inet.h> // ntohs/l, htons/l, inet_ntop, inet_pton
#include <netinet/in.h> // sockaddr_in, sockaddr_in6, in_addr, in6_addr
#include <sys/socket.h> // socket, bind, listen, accept, connect AF_INET, SOCK_STREAM

#define SERV_ADDR "0.0.0.0"
#define SERV_PORT 8888
#define SERV_BACKLOG 10
#define SERV_BUF 8

struct server_sock
{
    int sd;
    struct sockaddr_in addr;
    int backlog;
};

struct client_sock
{
    int sd;
    struct sockaddr_in addr;
};

// socket descriptor
int create_sock(struct server_sock *server);

// initialize sockaddr_in struct
int init_sock(struct server_sock *server, char *ip, short port);

// bind socket to interface and port
int bind_sock(struct server_sock *server);

// accept next connection
int accept_client(int sd, struct client_sock *client);

// listen with backlog
int listen_sock(struct server_sock *server);

// close socket
int close_client(struct client_sock *client);

// read line from socket
char *read_line(struct client_sock *client);

// send a file to socket
int send_file(char *path, struct client_sock *client);

// handle client socket
void handle_client(struct client_sock *client);

// print server struct
void print_server(struct server_sock *server);

// print client struct
void print_client(struct client_sock *client);

// print sockaddr_in struct
void print_addr(struct sockaddr_in *addr);

int main()
{
    struct server_sock server;
    int ret;

    if(create_sock(&server) == -1)
    {
        perror("create_sock");
        return 1;
    }

    if((ret = init_sock(&server, SERV_ADDR, SERV_PORT)) == -1)
    {
        perror("init_addr");
        return 1;
    }
    else if(ret == 0)
    {
        fprintf(stderr, "init_addr: invalid ip address\n");
        return 1;
    }
    else
        print_server(&server);
 
    if(bind_sock(&server) == -1)
    {
        perror("bind_sock");
        return 1;
    }

    if(listen_sock(&server) == -1)
    {
        perror("listen_sock");
        return 1;
    }

    while(1)
    {
        struct client_sock client;

        if(accept_client(server.sd, &client) == -1)
            perror("accept_client");
        else
            print_client(&client);

        /*
        // fork child
        pid_t pid = fork();

        // wait on the children
        if(pid == -1)
            perror("fork");
        // child sends response
        else if(pid == 0)
            handle_client(&client);
        */

        handle_client(&client);

        if(close_client(&client) == -1)
            perror("close_client");
    } 
}

void handle_client(struct client_sock *client)
{
    char *line = read_line(client);

    if(line == NULL)
        return;
 
    if(send_file(line, client) == -1)
        return;
    
    printf("done handling client");
}

char *read_line(struct client_sock *client)
{
    int cap = SERV_BUF, len = 0, done = 0, n;
    char *buf = (char *) malloc(cap);
    
    while(done == 0)
    {
        if(len == cap)
        {
            cap *= 2;

            if((buf = (char *) realloc(buf, cap)) == NULL)
            {
                perror("realloc");
                return NULL;
            }
        }        

        if((n = recv(client->sd, buf + len, 1, 0)) == -1 || n == 0)
        {
            perror("recv");
            return NULL;
        }

        if(buf[len] == '\n')
            done = 1;

        len++;
    }
    
    // null terminator included overwrites the newline
    buf[len - 1] = '\0';
    return buf;
}

int send_file(char *path, struct client_sock *client)
{
    printf("%s\n", path);

    int fd = open(path, O_RDONLY, 0644);
    if(fd == -1)
    {
        perror("open");
        return -1;
    }

    char c;
    int ret;    

    while((ret = read(fd, &c, 1)) != 0)
    {
        if(ret == -1)
        {
            if(errno == EINTR)
                continue;

            perror("read");
            return -1;
        }

        while((ret = send(client->sd, &c, 1, 0)) != -1)
        {
            if(ret == 0)
                continue;

            break;
        }

        if(ret == -1)
        {
            perror("send");
            return 1;
        }
    }

    return 0;
}

int create_sock(struct server_sock *server)
{
    server->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return server->sd;
}

int init_sock(struct server_sock *server, char *ip, short port)
{ 
    server->backlog = SERV_BACKLOG;

    memset(&server->addr, '\0', sizeof server->addr);
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(port);

    return inet_pton(AF_INET, ip, &server->addr.sin_addr);
}

int bind_sock(struct server_sock *server)
{
    return bind(server->sd, (struct sockaddr *) &server->addr, sizeof server->addr);
}

int listen_sock(struct server_sock *server)
{
    return listen(server->sd, server->backlog);
}

int accept_client(int sd, struct client_sock *client)
{
    socklen_t size = sizeof client->addr;
    client->sd = accept(sd, (struct sockaddr *) &client->addr, &size);
    return client->sd;
}

int close_client(struct client_sock *client)
{
    return close(client->sd);
}

void print_server(struct server_sock *server)
{
    printf("(server) ");
    print_addr(&server->addr);
}

void print_client(struct client_sock *client)
{
    printf("(client) ");
    print_addr(&client->addr);
}

void print_addr(struct sockaddr_in *addr)
{
    char ip[INET_ADDRSTRLEN];
    unsigned short port = ntohs(addr->sin_port);

    if(inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN) == NULL)
        perror("inet_ntop");
    else
        printf("ip: %s, port: %hu\n", ip, port);
}
