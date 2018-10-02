#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/**
 * Semaphores are variables which track the number of openings for a resource.
 *
 * Although not included in Pthreads, Linux systems usually have a semaphore
 * implementation which can be linked with -lrt.
 *
 * // decrement
 * int sem_wait(sem_t *sem);
 * 
 * // increment
 * int sem_post(sem_t *sem);
 *
 * // intialise
 * int sem_init(sem_t *sem, int pshared, unsigned int value);
 *
 * // destroy
 * int sem_destroy(sem_t * sem);
 */

sem_t mutex;

void *work(void *arg)
{
    sem_wait(&mutex);
    printf("work: entered mutex\n");
    
    sleep(4);
    
    printf("work: exiting mutex\n");
    sem_post(&mutex);
}

int main()
{
    sem_init(&mutex, 0, 1);
    pthread_t t1, t2;

    pthread_create(&t1, NULL, work, NULL);
    sleep(2);
    pthread_create(&t2, NULL, work, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    sem_destroy(&mutex);

    return 0;
}
