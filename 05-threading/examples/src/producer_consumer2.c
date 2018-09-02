#include <stdio.h>
#include <pthread.h>

#define MAX 20

int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;

void put(int value)
{
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % MAX;
    ++count;
}

int get()
{
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    --count;
    return tmp;
}

pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex;

/*
 * produce one and signal, waiting for a removal when full
 */
void *producer(void *p)
{
    int n = *(int *) p;

    for(int i=0; i<n; ++i)
    {
        pthread_mutex_lock(&mutex);

        while(count == MAX)
            pthread_cond_wait(&empty, &mutex);    

        put(i); 
        printf("produced: %d\n", i);

        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/*
 * consume one and signal, waiting for an insertion when empty
 */
void *consumer(void *p)
{
    int n = *(int *) p;

    for(int i=0; i<n; ++i)
    {
        pthread_mutex_lock(&mutex);

        while(count == 0)
            pthread_cond_wait(&fill, &mutex);

        int tmp = get();
        printf("consumed: %d\n", tmp);

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int loops = 10;
int main()
{
    pthread_t p, c;
    pthread_create(&p, NULL, producer, &loops);
    pthread_create(&c, NULL, consumer, &loops);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    return 0;
    
}
