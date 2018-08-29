#include <stdio.h>
#include <pthread.h>

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit()
{
    pthread_mutex_lock(&m);

    /*
     * pthread_cond_signal unblocks at least 1 sleeping thread
     *
     * in the event that more than one threads wakes up (spurious), application
     * code should check the condition once more (while loop) to ensure that is
     * true before proceeding and sleep otherwise
     */
    done = 1;
    pthread_cond_signal(&c);

    pthread_mutex_unlock(&m);
}

void *child(void *p)
{
    printf("child\n");
    thr_exit();
    return NULL;
}

void thr_join()
{
    pthread_mutex_lock(&m);

    while(!done)
        pthread_cond_wait(&c, &m);

    pthread_mutex_unlock(&m);
}

int main()
{
    printf("parent\n");
    pthread_t t;
    pthread_create(&t, NULL, child, NULL);

    thr_join();
    printf("joined\n");
    return 0;
}
