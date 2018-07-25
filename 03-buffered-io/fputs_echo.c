#include <stdio.h>
#include <string.h>
#include <limits.h>

/*
* int fputs(const char *str, FILE *stream);
*/
int main()
{
  char buf[LINE_MAX];

  if(fputs("Enter lines and they will be echoed ('q' to quit).\n", stdout) == EOF)
  {
    perror("fputs");
    return 1;
  }

  while(fgets(buf, LINE_MAX, stdin) != NULL)
  {
    if(strcmp(buf, "q\n") == 0)
      return 0;

    if(fputs(buf, stdout) == EOF)
    {
      perror("fputs");
      return 1;
    }
  }

  if(ferror(stdin))
  {
    perror("fgets");
    return 1;
  }
  else
  {
    printf("received EOF\n");
  }
}
