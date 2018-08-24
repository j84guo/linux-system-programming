#include <stdio.h>

int main()
{
    /*
    * const variable
    */
    const int a = 1;
    // compiler error, a is const
    // a = 2;
    
    /*
    * pointer to const variable
    */
    int b = 2;
    const int *bp = &b;
    printf("%d\n", *bp);    

    // compiler error, bp is a pointer to const int
    // *bp = 10;

    // allowed, since although the object is const, the pointer is not
    bp = NULL; 

    /*
    * const pointer to variable
    */
    int c = 3;
    int * const cp = &c;
    printf("%d\n", *cp);

    // allowed, since although the pointer is const, the object is not
    *cp = 4;

    // compiler error, cp is a const pointer to int
    // cp = NULL;

    /*
    * array of const (pointers to chars)
    */
    char *s = "hello";
    char *const v[] = {s, "there", "world"};
    
    // compiler error, v is a array of const strings
    // v[0] = "jackson";

    for(int i=0; i<3; ++i)
        printf("%s\n", v[i]);
}
