#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *inc(void *p);

int x = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    int ret1, ret2;
    pthread_t t1, t2;

    if((ret1 = pthread_create(&t1, NULL, inc, NULL)) != 0)
    {
        fprintf(stderr, "thread creation failed: %d\n", ret1);
    }

    if((ret2 = pthread_create(&t2, NULL, inc, NULL)) != 0)
    {
        fprintf(stderr, "thread creation failed: %d\n", ret2);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

void *inc(void *p)
{
    pthread_mutex_lock(&mutex);
    x++;
    printf("x: %d\n", x);
    pthread_mutex_unlock(&mutex);

    return NULL;
}
