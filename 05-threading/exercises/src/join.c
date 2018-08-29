#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMTHREADS 10

void *work(void *p);

// mutex and associated data have the same scope
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 0;

int main()
{
    pthread_t pool[NUMTHREADS];

    for(int i=0; i<NUMTHREADS; ++i)
        pthread_create(&pool[i], NULL, work, NULL);

    for(int i=0; i<NUMTHREADS; ++i)
        pthread_join(pool[i], NULL);

    printf("x: %d\n", x);    
}

void *work(void *p)
{
    // actually returns implementation dependent type pthread_t
    printf("thread number: %ld\n", pthread_self());

    pthread_mutex_lock(&mutex);
    x++;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
