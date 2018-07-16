#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 500

void print_fd(int fd);
void print_args(int argc, char** argv);

/*
* echo args... prints args to stdout
* or uses stdin if nothing passed in
*/
int main(int argc, char** argv)
{
  if(argc == 1)
    print_fd(STDIN_FILENO);
  else
    print_args(argc, argv);
}

void print_fd(int fd)
{
  char buffer[BUFFER_SIZE + 1];
  int nr;

  while((nr = read(fd, buffer, BUFFER_SIZE)) != 0)
  {
    if(nr == -1)
    {
      if(errno == EINTR)
        continue;

      perror("read");
      return;
    }

    buffer[nr] = '\0';
    printf("%s", buffer);
  }
}

void print_args(int argc, char** argv)
{
  for(int i=1; i<argc; ++i)
    printf("%s ", argv[i]);
    
  printf("\n");
}
