#include <stdio.h>

void insertion_sort(double *arr, size_t a, size_t b)
{
    for(size_t i=a+1; i<b; ++i) {
        double tmp = arr[i];

        for(size_t j=i; j>a; --j) {
            if(arr[j-1] > tmp)
                arr[j] = arr[j-1];
            else {
                arr[j] = tmp;
                break;
            }
        }

        /**
         * If tmp was smaller than all other elements, we'll have shifted the
         * sorted portion right by one without inserting tmp, therefore we do
         * that here. If tmp was already inserted, it cannot be < arr[a];
         */ 
        if(arr[a] > tmp)
            arr[a] = tmp;
    }
}

int main()
{
    double arr[] = {
        10, 5, 23
    };
    size_t len = sizeof(arr) / sizeof(double);

    insertion_sort(arr, 0, len - 1);

    for(size_t i=0; i<len; ++i)
        printf("%lf\n", arr[i]);
}
