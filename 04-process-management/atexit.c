#include <stdio.h>
#include <stdlib.h>

void out()
{
    printf("atexit() succeeded\n");
}

int main()
{
    if(atexit(out))
        fprintf(stderr, "atexit() failed\n");
}

