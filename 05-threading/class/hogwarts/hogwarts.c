#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <openssl/sha.h>

#define NUM_ELVES 5
#define HASH_BUFFER_LENGTH 32

// random number generator seed
unsigned int random_seed = 252;

// next task id
int task_id_counter = 0;

// task id and string input
typedef struct {
    int id;
    unsigned char *input;
} task;

// singly linked list of tasks
typedef struct node {
    task *task;
    struct node* next;
} node;

node* list_head;

// remaining work
int total_tasks;

// available tasks
int active_tasks;

// access global variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// signal dobby to refill
sem_t empty_list;

// signal workers to continue
sem_t full_list;

// dobby and elf functions
void *run_dobby(void *);
void *run_house_elf(void *);

// house elf gets a pointer to the next task (not thread safe)
task* take_task();

// house elf does a task
void do_work(task *todo);

// add some number of tasks to the queue
void post_tasks(int howmany);

// unlock mutex if the thread being cancelled has one
void house_elf_cleanup(void *);

/**
 * 1. argument is total number of tasks today
 * 2. intialize globals
 * 3. create 5 workers and 1 dobby
 * 4. if the queue is empty, workers signal empty_list and wait on full_list
 *
 *    workers take tasks using synchronized take_task() and then
 *    do the work using do_work()
 *
 *    decrement active_tasks
 *    signal full_list (misnomer, should be exists?)
 *
 * 5. when awoken, dobby posts up to 10 tasks (or remainder)
 *    updates active_tasks
 *    waits on empty_list
 *
 * 6. eventually dobby wakes up with total tasks equal to zero,
 *    so he cancels the workers and waits on them, the main thread waits
 *    on dobby and will exit when dobby does
 */
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s total_tasks\n", argv[0]);
        return -1;
    }

    // init global variables here
    list_head = NULL;
    total_tasks = atoi(argv[1]);

    // list starts empty, not full
    sem_init(&empty_list, 0, 1);
    sem_init(&full_list, 0, 0);
    printf("There are %d tasks to do today.\n", total_tasks);

    // launch doby
    pthread_t dobby;
    pthread_create(&dobby, NULL, run_dobby, NULL);

    // wait for doby to be done
    pthread_join(dobby, NULL);

    // cleanup global variables (mutex and sempahores)
    sem_destroy(&empty_list);
    sem_destroy(&full_list);

    return 0;
}

void *run_dobby(void *arg)
{
    // start workers
    pthread_t workers[5];
    for (int i = 0; i < 5; ++i) {
        pthread_create(&workers[i], NULL, run_house_elf, NULL);
    }

    while (1) {
        sem_wait(&empty_list);

        if (total_tasks == 0) {
            break;
        }

        // post more work
        int n = total_tasks >= 10 ? 10 : total_tasks;
        total_tasks -= n;
        post_tasks(n);
        active_tasks = n;

        // increment semaphore n times
        for (int i = 0; i < n; ++i) {
            sem_post(&full_list);
        }
    }

    // cancel and join workers
    for (int i=0; i<5; ++i) {
        pthread_cancel(workers[i]);
        pthread_join(workers[i], NULL);
    }
}

/**
 * Note that it looks like multiple workers may see active_tasks == 0 and try
 * to signal Dobby. This doesn't actually happen though, since (1) Dobby only
 * increments the semaphore active_tasks number of times and (2) a worker with
 * access to the shared variables is in the critical section where the next
 * task is taken and the variables updated. So a worker who sees 0 is the last
 * worker in the current batch, all others area working on their task and will
 * loop back to the semaphore.
 *
 * Another solution has the workers block inside the mutex as they check the
 * number of tasks left. If zero, the worker posts dobby and waits on full_list
 * (blocking all the other workers as it waits). The worker takes a task,
 * updates global variables, unlocks the mutex and works.
 */
void *run_house_elf(void *ignore)
{ 
    while (1) {
        sem_wait(&full_list);

        // register mutex unlock handler and lock mutex
        // since lock is not a cancellation point, there is no risk of cancel
        // happening after push and before lock (assuming deferred type)
        pthread_cleanup_push(house_elf_cleanup, NULL);
        pthread_mutex_lock(&mutex);

        b = 1;
        task *todo = take_task();
        --active_tasks;
        if (active_tasks == 0) {
            printf("Waking Dobby\n");
            sem_post(&empty_list);
        }

        // pop handler and unlock mutex, could also pass 1 to do it on 1 line
        pthread_mutex_unlock(&mutex);
        pthread_cleanup_pop(0);

        do_work(todo);
    }
}

void house_elf_cleanup(void *arg)
{
    printf("unlocking mutex\n");
    pthread_mutex_unlock(&mutex);
}

// generate random string code based off original by Ates Goral
char *random_string(const int len, unsigned int* generator_seed)
{
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";

    char* s = malloc((len + 1) * sizeof(char));
    for (int i=0; i<len; ++i) {
        s[i] = alphanum[rand_r(generator_seed) % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
    return s;
}

// adds a bunch of new tasks
void post_tasks(int howmany)
{
    printf("Adding %d tasks to the list of active tasks.\n", howmany);

    for (int i=0; i<howmany; ++i) {
        task* t = malloc(sizeof(task));
        t->id = ++task_id_counter;
        t->input = random_string(HASH_BUFFER_LENGTH, &random_seed);
        node *n = malloc(sizeof(node));
        n->task = t;
        n->next = list_head;
        list_head = n;
    }
}

// do a task
void do_work(task *todo)
{
    unsigned char* output_buffer = calloc(HASH_BUFFER_LENGTH, sizeof(unsigned char));
    SHA256(todo-> input, HASH_BUFFER_LENGTH, output_buffer);
    printf("Task %d was completed!\n", todo - > id);
    free(output_buffer);
    free(todo->input);
    free(todo);
}

// get next task
task *take_task()
{
    if (list_head == NULL) {
        /* Something has gone wrong */
        printf("Trying to take from an empty list!\n");
        exit(-1);
    }

    node *head = list_head;
    task *t = head->task;
    list_head = list_head->next;
    free(head);
    return t;
}
