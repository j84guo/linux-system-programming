#include <stdio.h>
#include <limits.h>

int main()
{
  char buf[LINE_MAX];

  if(fgets(buf, LINE_MAX, stdin) == NULL)
  {
    perror("fgets");
    return 1;
  }
  else
  {
    /*
    * notice that a segmentation fault occurs if an integer is formatted as %s
    */
    printf("read line of up to %d bytes (counting '\\0' and including '\\n'): %s", LINE_MAX, buf);
  }
}
