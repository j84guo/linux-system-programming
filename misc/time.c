#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main()
{
    struct timeval tv;
    double t1, t2;

    gettimeofday(&tv, NULL);
    t1 = tv.tv_sec + tv.tv_usec/1000000.0;

    sleep(1);
    usleep(50);

    gettimeofday(&tv, NULL);
    t2 = tv.tv_sec + tv.tv_usec/1000000.0;
    
    printf("%lf seconds elapsed\n",  t2 - t1);
    return 0;
}
