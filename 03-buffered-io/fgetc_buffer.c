#include <stdio.h>
#include <stdlib.h>

#define INIT_BUF 32;

/*
* int fgetc(FILE *stream);
*
* see also ungetc(), which puts a byte back onto the stream
*/
int main(int argc, char** argv)
{
  if(argc != 2)
  {
    fprintf(stderr, "usage: ./fgetc_buffer <path>\n");
    return 1;
  }

  /*
  * open stream
  */
  FILE *stream = fopen(argv[1], "r+");
  if(stream == NULL)
  {
    perror("fopen");
    return 1;
  }

  /*
  * read byte by byte into buffer, checking for read error
  * note that the buffer is made one larger than its capacity in order to hold a
  * null terminator
  */
  int cap = INIT_BUF;
  char *buf = (char*) malloc(sizeof(char) * cap + 1);
  int len = 0;
  int c;

  while((c = fgetc(stream)) != EOF)
  {
    if(len == cap)
    {
      char *tmp = (char*) malloc(sizeof(char) * cap * 2 + 1);

      for(int i=0; i<cap; ++i)
        tmp[i] = buf[i];

      cap *= 2;
      free(buf);
      buf = tmp;
    }

    buf[len++] = c;
  }

  if(ferror(stream))
  {
    perror("fgetc");
    return 1;
  }

  buf[len] = '\0';
  printf("%s", buf);

  /*
  * free the buffer to avoid memory leak
  */
  free(buf);
}
