#include <stdio.h>

int main()
{

    int x = 5;

    {
        x = 7;
    }

    printf("%d\n", x);
}
