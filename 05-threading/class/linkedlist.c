#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int A[] = {1, 2, 3, 4, 5};

/**
 * singly linked list node
 */
struct single_node {
    void *element;
    struct single_node *next;
};

/**
 * singly linked list
 */
struct single_list {
    struct single_node *head;
    struct single_node *tail;
    int size;
    sem_t sem;
};

/**
 * initialize head, tail and size of singly linked list
 */
int single_list_init(struct single_list *list)
{
    if(list == NULL)
        return 0;

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    sem_init(&list->sem, 0, 1);
}

/**
 * push a node to the front
 */
int push_front(struct single_list *list, void *obj)
{
    struct single_node *tmp = malloc(sizeof(struct single_node));

    if(tmp == NULL)
        return 0;

    tmp->element = obj;
    
    sem_wait(&list->sem);
    tmp->next = list->head;
    list->head = tmp;

    if(list->size == 0)
        list->tail = tmp;

    ++(list->size);
    sem_post(&list->sem);

    return 1;
}

/**
 * free linked list nodes
 */
int single_list_free(struct single_list *list)
{
    struct single_node *node = list->head;
    struct single_node *next;

    if(list == NULL)
        return 0;

    while(node != NULL)
    {
        next = node->next;
        free(node);
        node = next;
    }

    return 1;
}

/**
 * push some values into linked list
 */
void *push_thread(void *arg)
{
    struct single_list *list = (struct single_list *) arg;
   
    for(int i=0; i<5; ++i) {
        push_front(list, &A[i]);
        sleep(1);
    }   
}

int main()
{
    
    pthread_t t1, t2;
    struct single_list list;
    struct single_node *node;

    single_list_init(&list);
    pthread_create(&t1, NULL, push_thread, &list);
    pthread_create(&t2, NULL, push_thread, &list);

    pthread_join(t2, NULL);
    pthread_join(t1, NULL);
    node = list.head;

    while(node != NULL)
    {
        printf("%d\n", *((int *) node->element));
        node = node->next;
    }

    single_list_free(&list);
}
