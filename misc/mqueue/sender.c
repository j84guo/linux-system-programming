#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#include "common.h"
#include "point.h"

int main(int argc, char **argv)
{
    mqd_t qdes;
    char quit = '\0';

    char *qname;
    mode_t mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr;

    if (argc != 2) {
        printf("Usage: %s <qname>\n", argv[0]);
        printf("The qname must start with a \"/\". \n");
        exit(1);
    }

    qname = argv[1];
    
    attr.mq_maxmsg = QUEUE_SIZE;
    attr.mq_msgsize = sizeof(struct point);
    attr.mq_flags = 0;

    qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
    if (qdes == -1) {
        perror("mq_open");
        exit(1);
    }

    srand(time(NULL));

    do {
        quit = getchar();
        struct point pt;

        set_position(rand() % 80, rand() % 24, &pt);
        if (mq_send(qdes, (char *) &pt, sizeof(struct point), 0) == -1) {
            perror("mq_send");
        }
        printf("sending random point: %d, %d", get_x_coord(pt),
            get_y_coord(pt));
    } while (quit != 'q');

    printf("Press ENTER to send a point, q + ENTER to quit\n");
    
    if (mq_close(qdes) == -1) {
        perror("mq_close");
        exit(2);
    }

    if (mq_unlink(qname) != 0) {
        perror("mq_unlink");
        exit(3);
    }

    return 0;
}
