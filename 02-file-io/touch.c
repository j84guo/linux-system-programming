#include <sys/types.h> // mode_t
#include <sys/stat.h> // permission bits (S_IRUSR)
#include <unistd.h> // close()
#include <fcntl.h> // open(), open options (O_CREAT)
#include <stdio.h> // printf()

mode_t default_mode();

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    fprintf(stderr, "usage: ./touch <path>\n");
    return 1;
  }

  int fd;

  if((fd = open(argv[1], O_CREAT, default_mode())) == -1)
  {
    perror("open");
    return 1;
  }

  if(close(fd) == -1)
  {
    perror("close");
    return 1;
  }

  return 0;
}

mode_t default_mode()
{
  return S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
}
