#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int fd;
    struct sockaddr_in addr;
} tcpcon_t;

int tcpcon_config(tcpcon_t *conn, char *ip, unsigned short port)
{
    if (conn == NULL)
        return -1;

    if (inet_pton(AF_INET, ip, &conn->addr.sin_addr) != 1)
        return -1;
    
    conn->addr.sin_family = AF_INET;      
    conn->addr.sin_port = htons(port);
    memset(&conn->addr.sin_zero, 0, sizeof conn->addr.sin_zero);

    if ((conn->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        return -1;

    return 0;
}

int tcpcon_destroy(tcpcon_t *conn)
{
    if (conn == NULL)
        return -1;

    return close(conn->fd);
}

int tcpcon_init(tcpcon_t *conn, char *ip, unsigned short port)
{
    if (tcpcon_config(conn, ip, port) == -1)
        return -1;

    if (connect(conn->fd, (struct sockaddr *) &conn->addr,
        sizeof conn->addr) == -1) {
        return -1;
    }

    return 0;
}

int sendall(int fd, char *buf, int len)
{
    int i = 0, n;
    
    while (i < len) {
        if ((n = send(fd, buf, len, 0)) == -1)
            return -1;

        i += n;
    }

    return 0;
}

int loop(tcpcon_t *conn)
{
    int n;
    char rbuf[512 + 1], wbuf[512];

    while (1) {
        if (fgets(wbuf, 512, stdin) == NULL) {
            if (ferror(stdin)) {
                perror("fgets");
                return -1;
            }

            break;
        }

        if (sendall(conn->fd, wbuf, strlen(wbuf)) == -1) {
            perror("sendall");
            return -1; 
        }

        n = recv(conn->fd, rbuf, 512, 0);

        if (n == -1) {
            perror("recv");
            return -1;
        } else if (n == 0) {
            printf("Server disconnected\n");
            return 0;
        }
         
        rbuf[n] = '\0';   
        printf("%s", rbuf);
    }

    printf("Closing connection\n");
    return 0;
}

int main()
{
    printf("Establishing connection\n");
    tcpcon_t conn;
    int res = tcpcon_init(&conn, "127.0.0.1", 8888);
    if (res) {
        perror("tcpcon_init");
        return 1;
    }

    res = loop(&conn);
    tcpcon_destroy(&conn);
    return res;
}
