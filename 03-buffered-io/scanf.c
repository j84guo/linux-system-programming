#include <stdio.h>

/*
* int scanf(const char* format, ...);
*
* assigns values to the objects pointed to by the pointer parameters by parsing
* stdin based on a format
*
* returns the number of elements assigned, stopping at the first matching error,
* or EOF, sets ferror and feof appropriately
*/
int main()
{
  char str[80];
  int i;
  int r;

  printf("Enter your family name: ");
  scanf("%79s", str);

  printf("Enter your age: ");
  r = scanf("%d", &i);
  if(r == EOF || r < 1)
    fprintf(stderr, "scanf: could not parse age\n");

  printf("");
}
