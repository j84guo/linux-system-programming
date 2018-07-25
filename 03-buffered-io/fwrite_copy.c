#include <stdio.h>

#include <fcntl.h>

int main(int argc, char** argv)
{
  if(argc != 3)
  {
    fprintf(stderr, "usage: ./fwrite_copy <src_name> <dst_name>");
    return 1;
  }

  char* src_name = argv[1];
  char* dst_name = argv[2];

  FILE* src_stream = fopen(src_name, "r");
  if(src_stream == NULL)
  {
    perror("fopen");
    return 1;
  }

  int fd = open(dst_name, O_WRONLY | O_CREAT | O_EXCL, 0644);
  if(fd == -1)
  {
    perror("open");
    return 1;
  }

  FILE* dst_stream = fdopen(fd, "r");
  if(dst_stream == NULL)
  {
    perror("fopen");
    return 1;
  }

  // read and write
}
