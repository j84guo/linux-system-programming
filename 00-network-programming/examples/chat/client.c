#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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

typedef struct {
    char ip[INET6_ADDRSTRLEN];
    unsigned short port;
} tcpinfo_t;

int tcpcon_config(tcpcon_t *conn, tcpinfo_t *info)
{
    if (!conn || !info)
        return -1;

    if (!inet_pton(AF_INET, info->ip, &conn->addr.sin_addr)) {
        fprintf(stderr, "tcpcon_init: invalid ip format\n");
        return -1;
    }

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port = htons(info->port);
    memset(&conn->addr.sin_zero, 0, sizeof conn->addr.sin_zero);

    if ((conn->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket");
        return -1;
    }

    return 0;
}

int tcpcon_destroy(tcpcon_t *conn)
{
    if (!conn)
        return -1;

    return close(conn->fd);
}

int tcpcon_init(tcpcon_t *conn, tcpinfo_t *info)
{
    int res = tcpcon_config(conn, info);
    if (res != 0)
        return res;

    if (connect(conn->fd, (struct sockaddr *) &conn->addr,
        sizeof conn->addr) == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}

int sendall(int fd, char *buf, int len)
{
    int i = 0, n;

    while (i < len) {
        if ((n = send(fd, buf, len, 0)) == -1) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        i += n;
    }

    return 0;
}

int loop(tcpcon_t *conn)
{
    int n;
    char rbuf[512 + 1];

    while (1) {
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

    return 0;
}

void *run_eloop(void *arg)
{
    loop((tcpcon_t *) arg);
    return NULL;
}

void stop_thread(pthread_t tid)
{
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

void tcpinfo_init(tcpinfo_t *info, int argc, char **argv)
{
    strncpy(info->ip, argv[1], INET6_ADDRSTRLEN);
    info->port = atoi(argv[2]);
}

/**
 * Todo:
 * - indicator socket
 * - select vs poll usage
 * - chat protocol
 */
int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
        return 1;
    } else if (strlen(argv[1]) > INET6_ADDRSTRLEN) {
        fprintf(stderr, "usage: <ip> less then  %d bytes\n", INET6_ADDRSTRLEN);
        return 1;
    }

    tcpinfo_t info;
    tcpinfo_init(&info, argc, argv);

    printf("Opening connection\n");
    tcpcon_t conn;
    if (tcpcon_init(&conn, &info))
        return 1;

    pthread_t eloop;
    pthread_create(&eloop, NULL, run_eloop, &conn);

    char input[512];
    while (1) {
        if (!fgets(input, 512, stdin)) {
            if (ferror(stdin)) {
                perror("fgets");
                stop_thread(eloop);
                return 1;
            }

            break;
        }

        printf("stdin: %s", input);
    }

    printf("Closing connection\n");
    stop_thread(eloop);
    tcpcon_destroy(&conn);
    return 0;
}
