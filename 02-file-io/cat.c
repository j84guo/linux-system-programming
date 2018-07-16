#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#define READ_SIZE 500

void print_file(int fd);

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    fprintf(stderr, "usage: ./cat <path>\n");
    return 1;
  }

  int fd;

  if((fd = open(argv[1], O_RDONLY)) == -1)
  {
    perror("open");
    return 1;
  }

  print_file(fd);

  if(close(fd) == -1)
  {
    perror("close");
    return 1;
  }
}

/*
* reads and prints file
*/
void print_file(int fd)
{
  char buffer[READ_SIZE + 1];
  int nr;

  while((nr = read(fd, buffer, READ_SIZE)) != 0)
  {
    if(nr == -1)
    {
      if(errno == EINTR)
        continue;

      perror("ERROR");
      return;
    }
    else
    {
      buffer[nr] = '\0';
      printf("%s", buffer);
    }
  }
}
