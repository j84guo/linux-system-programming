#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define BUFCAP 20
#define REFILLS 3

int buf[BUFCAP];
int n = 0;

/*
 * lock access to buffer
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * consumer waits until full
 */
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;

/*
 * producer waits until empty
 */
pthread_cond_t empty_cond = PTHREAD_COND_INITIALIZER;

void *consume(void *p);
void *produce(void *p);

void insert_one();
void signal_and_wait();
void remove_all();

int main()
{
    pthread_t t1, t2;

    /*
     * producer is joinable
     */
    pthread_create(&t1, NULL, produce, NULL);

    /*
     * consumer is detached
     */
    pthread_create(&t2, NULL, consume, NULL);
    pthread_detach(t2);

    pthread_join(t1, NULL);
    return 0;
}

void *produce(void *p)
{
    int i=0, done=0;
    // sleep(3);

    while(1)
    { 
        pthread_mutex_lock(&mutex);
        insert_one();
        
        if(n == BUFCAP)
        {
            signal_and_wait();
            if(++i == REFILLS)
                done = 1;
        }

        pthread_mutex_unlock(&mutex);
        if(done == 1)
            return NULL;
    }
}

void insert_one()
{
    printf("adding at: %d\n", n);        
    buf[n] = n;
    ++n;
}

void signal_and_wait()
{
    pthread_cond_signal(&full_cond);

    while(n > 0)
        pthread_cond_wait(&empty_cond, &mutex);
}

void *consume(void *p)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);

        while(n < BUFCAP)
            pthread_cond_wait(&full_cond, &mutex);

        remove_all();

        pthread_cond_signal(&empty_cond);
        pthread_mutex_unlock(&mutex);            
    }
}

void remove_all()
{
    while(n > 0)
        printf("n: %d\n", n--);
}
