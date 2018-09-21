#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum = 0;

void *runner(void *param) {
    int i, upper = atoi(param);

    sum = 0;
    for(i=1; i<=upper; ++i) {
        sum += i;
    }

    pthread_exit(0);
}

int main(int argc, char **argv)
{
    pthread_t tid;
    pthread_attr_t attr;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <int>\n", argv[0]);
        return 1;
    }

    int x = atoi(argv[0]);
    if(x < 0) {
        fprintf(stderr, "%d must be >= 0", x);
        return 1;
    }

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, NULL);
    printf("sum=%d\n", sum);
}


