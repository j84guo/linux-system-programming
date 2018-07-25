#include <stdio.h>

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    fprintf(stderr, "usage: ./open_close <path>\n");
    return 1;
  }

  /*
  * notice the fopen() does not provide mode_t mode argument for file
  * permissions, this is because the function does not create new files
  */
  FILE *stream = fopen(argv[1], "r");

  if(stream == NULL)
  {
    perror("fopen");
    return 1;
  }

  if(fclose(stream) == EOF)
  {
    perror("fclose");
    return 1;
  }
}
