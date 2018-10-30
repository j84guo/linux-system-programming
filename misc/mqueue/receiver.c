#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <signal.h>

#include "common.h"
#include "point.h"

bool g_continue = true;

void sig_handler(int sig)
{
    g_continue = false;
}

/**
 * Link against librt using -lrt to access the mqueue api
 */
int main(int argc, char **argv)
{
    mqd_t qdes;
    char *qname;
    mode_t mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr;

    if (argc != 2) {
        printf("Usage: %s <qname>\n", argv[0]);
        printf("<qname> should start with a forwards slash \"/\"\n");
        exit(1);
    }

    qname = argv[1];
    
    attr.mq_maxmsg = QUEUE_SIZE;
    attr.mq_msgsize = sizeof(struct point);
    attr.mq_flags = 0;

    qdes = mq_open(qname, O_RDONLY, mode, &attr);
    if (qdes == -1) {
        perror("mq_open");
        exit(1);
    }

    signal(SIGINT, sig_handler);
    
    while (g_continue) {
        struct point pt;
        struct timespec ts = {
            time(0) + 5, 
            0
        };

        if (mq_timedreceive(qdes, (char *) &pt, 
            sizeof(struct point), 0, &ts) == -1) {
            perror("mq_timedreceive");
            fprintf(stderr, "Press Ctrl-C to terminate\n");
        } else {
            printf("Received random point %d, %d\n", get_x_coord(pt),
                get_y_coord(pt));
        }
    }

    if (mq_close(qdes) == -1) {
        perror("mq_close");
        exit(2);
    }

    return 0;
}
