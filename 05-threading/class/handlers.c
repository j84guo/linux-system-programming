#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Recevies arg from pthread_cleanup_push
 */
void cleanup(void *arg)
{
    printf("Running cleanup...\n");
    free(arg);
}

/**
 * pthread_cleanup_push(void (*func)(void *), void *arg);
 *
 * pthread_cleanup_pop(int execute);
 */
int main()
{
    int *j = malloc(sizeof(int));
    pthread_cleanup_push(cleanup, j);

    /**
     * Pop a handler and execute it. Handlers have to be popped before the
     * thread exists.
     */ 
    // pthread_cleanup_pop(1);    
}
