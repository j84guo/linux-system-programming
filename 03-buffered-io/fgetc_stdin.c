#include <stdio.h>

int main()
{
  int c = fgetc(stdin);

  if(c != EOF)
  {
    printf("read one character: %c\n", (char) c);
  }
  else if(ferror(stdin))
  {
    perror("fgetc");
    return 1;
  }
  else if(feof(stdin))
  {
    fprintf(stderr, "received EOF\n");
    return 1;
  }

  return 0;
}
