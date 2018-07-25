#include <stdio.h>

/*
* reads "nr" items of "size" bytes from "stream" into "buf", the stream position
* is advanced by the number of bytes read, which is also returned, failure or
* EOF is indicated via a return value less than nr, the speicific condition
* which occurred is tested using ferror() or feof()
*
* size_t fread(void *buf, size_t size, size_t nr, FILE *stream);
*/
int main()
{
  int cap = 8;
  char buf[cap + 1];
  size_t nr = fread(buf, cap, 1, stdin);

  if(nr < 1)
  {
    if(ferror(stdin))
    {
      perror("fread");
      return 1;
    }
    else
    {
      printf("received EOF\n");
      return 0;
    }
  }

  buf[cap] = '\0';
  printf("read exactly %d bytes from stdin: %s", cap, buf);
}
