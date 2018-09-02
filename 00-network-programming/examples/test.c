#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if(argc != 2)
        return 1;

    char *end;
    long l = strtol(argv[1], &end, 10);

    printf("%ld\n%s\n", l, end);
}
