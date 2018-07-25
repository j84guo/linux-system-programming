#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

mode_t sixFourFour();

/*
* FILE* fdopen(int fd, const char *mode);
*/
int main(int argc, char** argv)
{
  if(argc != 2)
  {
    fprintf(stderr, "usage: ./fdopen <path>\n");
    return 1;
  }

  int fd = open(argv[1], O_RDWR | O_CREAT, sixFourFour());
  if(fd == -1)
  {
    perror("open");
    return 1;
  }

  FILE *stream = fdopen(fd, "r+");
  if(stream == NULL)
  {
    perror("fdopen");
    return 1;
  }

  if(fclose(stream) != 0)
  {
    perror("fclose");
    return 1;
  }
}

mode_t sixFourFour()
{
  return S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
}
