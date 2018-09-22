#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>

struct ArrDouble
{
    double *A;
    size_t start;
    size_t end;
};

void insertion_sort(double *A, size_t a, size_t b)
{
    for(size_t i=a+1; i<b; ++i) {
        double tmp = A[i];

        for(size_t j=i; j>a; --j) {
            if(A[j-1] > tmp)
                A[j] = A[j-1];
            else {
                A[j] = tmp;
                break;
            }
        }

        if(A[a] > tmp)
            A[a] = tmp;
    }
}

void merge(double *A, size_t start, size_t end)
{
    if(start >= end)
        return;

    size_t mid = (start + end) / 2;

    size_t tmp_len = end - start + 1;
    double *tmp = malloc(sizeof(double) * tmp_len);
    int i = 0;
    size_t a_index=start, b_index=mid+1, a_end=mid, b_end=end;

    while(a_index <= a_end && b_index <= b_end) {
        if(A[a_index] < A[b_index]) {
            tmp[i] = A[a_index++];
        else
            tmp[i] = A[b_index++];

        ++i;
    }

    while(a_index <= a_end)
        tmp[i++] = A[a_index++];

    while(b_index <= b_end)
        tmp[i++] = A[b_index++];

    for(i=start; i<=end; ++i)
        A[i] = tmp[i - start];

    free(tmp);
}

void* merge_sort(void *p)
{
    struct ArrDouble arg = * (struct ArrDouble *) p;

    if(arg.start >= arg.end)
        return NULL;

    double start=arg.start, end=arg.end;

    /**
     * Since start + end could overflow our numeric type, we want to do the
     * equivalent start/2 + end/2. However this fails when both numbers are odd.
     * Although start + end would be even, we rounded start/2 and end/2 down, so
     * each operand loses 0.5 and our sum is 1 less than it should be.
     *
     * The solution is to do start + (end - start) / 2. Only if the sum of the
     * number sis odd do we want to roudn down by 1. Well it turns out that an
     * odd sum implies an odd difference, so we know we've roudned down the
     * right number of times.
     */
    size_t mid = start + (end-start) / 2;

    struct ArrDouble arg1=arg, arg2=arg;
    pthread_t t1, t2;

    arg1.end = mid;
    pthread_create(&t1, NULL, merge_sort, &arg1);

    arg2.start = mid +1;
    arg2.end = end;
    pthread_create(&t2, NULL, merge_sort, &arg2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    merge(arg.A, start, end);
}

int main()
{
    double A[] = {123, 231, 41, 98, 123, 432, 21, 2};
    size_t end = sizeof(A) / sizeof(double) - 1;

    struct ArrDouble arg = {A, 0, end};
    merge_sort(&arg);

    for(int i=0; i<=end; ++i)
        printf("%lf\n", A[i]);
}
