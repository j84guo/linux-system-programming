#include <stdio.h>
#include <limits.h>

/*
* char* fgets(char* str, int size, FILE* stream);
*/
int main()
{
  char buf[LINE_MAX];

  if(fgets(buf, LINE_MAX, stdin) != NULL)
  {
    /*
    * notice that a segmentation fault occurs if an integer is formatted as %s
    */
    printf("read one line of up to %d bytes (counting '\\0' and including '\\n'): %s", LINE_MAX, buf);
  }
  else if(ferror(stdin))
  {
    perror("fgets");
    return 1;
  }
  else
  {
    printf("received EOF\n");
    return 0;
  }
}
