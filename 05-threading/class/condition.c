#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void *inc_count(void *t)
{
    int i;
    long my_id = (long) t; // how can you be sure pointer is wide enough
    
    for (i=0; i<TCOUNT; ++i) {
        pthread_mutex_lock(&mutex);

        if (++count == COUNT_LIMIT) {
            printf("inc_count: threshold reached thread=%ld count=%d\n", my_id,
                count);
            pthread_cond_signal(&cond);
            printf("inc_count: just signalled\n");
        }
        printf("inc_count: unlocking mutex thread=%ld count=%d\n", my_id,
            count);

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_exit(NULL);
}

void *watch_count(void *t)
{
    long my_id = (long) t;
    printf("starting watch count: thread=%ld\n", my_id);

    pthread_mutex_lock(&mutex);
    /**
     * while loop handles spurious wakeups
     */
    while (count < COUNT_LIMIT) {
        printf("watch_count: thread=%ld count=%d going to wait...\n", my_id,
            count);
        pthread_cond_wait(&cond, &mutex);
        printf("watch_count: thread=%ld count=%d signal received...\n", my_id,
            count);
        count += 125;
        printf("watch_count: thread=%ld count=%d updated count\n", my_id,
            count);
    }

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

/**
 * two threads running inc_count each increment 10 times
 * one thread running watch_coutn increments 125 times when signalled
 * main will see a final value of 145 when all threads are joined
 */
int main(int argc, char **argv)
{
    int i, rc;
    long t1=1, t2=2, t3=3;

    pthread_t threads[3];
    pthread_attr_t attr;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); // very few systems will not enable joining by default
    pthread_create(&threads[0], &attr, watch_count, (void*) t1); // sketchy conversion
    pthread_create(&threads[1], &attr, inc_count, (void*) t2);
    pthread_create(&threads[2], &attr, inc_count, (void*) t3);
    
    for (i=0; i<NUM_THREADS; ++i)
        pthread_join(threads[i], NULL);
    
    printf("main: waited on %d threads count=%d\n", NUM_THREADS, count);
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
