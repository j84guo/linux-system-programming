#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <limits.h>

/*
* int fputc(int c, FILE *stream);
*/
int main()
{
  char *path = "line.txt";

  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
  if(fd == -1)
  {
    perror("open");
    return 1;
  }

  FILE *stream = fdopen(fd, "w");
  if(stream == NULL)
  {
    perror("fdopen");
    return 1;
  }

  char buf[LINE_MAX];
  buf[0] = '\0';

  if(fgets(buf, LINE_MAX, stdin) != NULL)
  {
    for(int i=0; i<strlen(buf); ++i)
    {
      if(fputc(buf[i], stream) == EOF)
      {
        perror("fputc");
        return 1;
      }
    }
  }
  else if(ferror(stdin))
  {
    perror("fgets");
    return 1;
  }
  else
  {
    printf("received EOF, not writing to file\n");
  }

  if(fclose(stream) == EOF)
  {
    perror("fclose");
    return 1;
  }
}
