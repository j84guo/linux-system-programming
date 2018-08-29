#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// pointer passed to pthread_create
void *print_msg(void *msg);

/*
 * int pthread_create(pthread_t * thread, 
 *                    const pthread_attr_t * attr,
 *                    void * (*start_routine)(void *), 
 *                    void *arg);
 */
int main()
{
    pthread_t t1, t2;

    char *msg1 = "hello from child thread 1";
    char *msg2 = "hello from child thread 2";

    // not checking return value
    int ret1 = pthread_create(&t1, NULL, print_msg, (void *) msg1);
    int ret2 = pthread_create(&t2, NULL, print_msg, (void *) msg2);
    
    // all threads terminate when the main thread falls off the main function
    // we wait for the child threads so they finish first
    // receives thread id by value
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("t1: %d\n", ret1);
    printf("t2: %d\n", ret2);

    return 0;
}

void *print_msg(void *msg)
{
    printf("%s\n", msg);
    return msg;
}
