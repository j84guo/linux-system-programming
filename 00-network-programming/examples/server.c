#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BACKLOG 20

void init_serv(struct sockaddr_in *serv);
void sigchild_handler(int s);
int reap_zombies();
void print_client(struct sockaddr_in *client);
void try_close(int fd);

int main(int argc, char **argv)
{
    struct sockaddr_in serv;
    init_serv(&serv);

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sd == -1)
    {
        perror("socket");
        return 1;
    }

    int val = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val) == -1)
    {
        perror("setsockopt");
        return 1;
    }

    if(bind(sd, (struct sockaddr *) &serv, sizeof serv) == -1)
    {
        perror("bind");
        return 1;
    }

    if(listen(sd, BACKLOG) == -1)
    {
        perror("listen");
        return 1;
    }

    if(reap_zombies() == -1)
    {
        perror("reap_zombies");
        return 1;  
    }    

    while(1)
    {
        struct sockaddr_in client;
        socklen_t size = sizeof client;

        int cd = accept(sd, (struct sockaddr *) &client, &size);
        print_client(&client);

        pid_t pid = fork();
        if(pid == -1)
        {
            perror("fork");
            try_close(cd);
        }
        else if(pid == 0)
        {
            try_close(sd);

            char *buf = "hello world";

            if(send(cd, buf, strlen(buf), 0) == -1)
                perror("send");

            try_close(cd);
            return 0;
        }
        else
        {
            /**
             * Prevents the parent from keeping the connection alive
             */ 
            try_close(cd);
        }
    }
}

void init_serv(struct sockaddr_in *serv)
{
    serv->sin_family = AF_INET;
    serv->sin_port = htons(PORT);
    serv->sin_addr.s_addr = INADDR_ANY;
    memset(&serv->sin_zero, 0, sizeof serv->sin_zero); 
}

void sigchld_handler(int s)
{
    int tmp = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = tmp;
}

int reap_zombies()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    return sigaction(SIGCHLD, &sa, NULL);
}

void print_client(struct sockaddr_in *client)
{
    char buf[INET6_ADDRSTRLEN];
    inet_ntop(client->sin_family, &client->sin_addr, buf, sizeof buf);
    printf("connected with %s:%hu\n", buf, client->sin_port);
}

void try_close(int fd)
{
    if(close(fd) == -1)
        perror("close");
}
