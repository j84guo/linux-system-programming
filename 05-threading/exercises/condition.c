#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// thread1 waits on this condition
pthread_mutex_t cond1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

// thread2 waits on this condition
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

// thread2 waits on this condition
void *funcCount1(void *p);
void *funcCount2(void *p);

int count = 0;

#define COUNT_DONE 10
#define COUNT_HALT1 3
#define COUNT_HALT2 6

int main()
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, funcCount1, NULL);
    pthread_create(&t2, NULL, funcCount2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

void *funcCount1(void *p)
{
    int done = 0;

    while(1)
    {
        pthread_mutex_lock(&cond1_mutex);

        while(count >= COUNT_HALT1 && count <= COUNT_HALT2)
        {
            pthread_cond_wait(&cond1, &cond1_mutex);
        }
 
        count++;
        printf("funcCount1: %d\n", count);

        if(count >= COUNT_DONE)
            done = 1;

        if(count >= COUNT_HALT1 &&  count <= COUNT_HALT2)
        {
            pthread_cond_signal(&cond2);
        }

        pthread_mutex_unlock(&cond1_mutex);

        if(done)
        {
            pthread_cond_signal(&cond2);
            return NULL;
        }
    } 
}

/*
 * todo: fix this buggy code
 */
void *funcCount2(void *p)
{
    int done = 0;

    while(1)
    {
        pthread_mutex_lock(&cond1_mutex);

        while(count < COUNT_HALT1 || (count > COUNT_HALT2 && count < COUNT_DONE))
        {
            pthread_cond_wait(&cond2, &cond1_mutex);
        }

        if(count == COUNT_DONE)
            return NULL;

        count++;                                                            
        printf("func2: %d\n", count);

        if(count >= COUNT_DONE)
            done = 1;

        if(count < COUNT_HALT1 || count > COUNT_HALT2)
        {
            pthread_cond_signal(&cond1);
        }

        pthread_mutex_unlock(&cond1_mutex);

        if(done)
        {
            return NULL;
        }
    }
}
