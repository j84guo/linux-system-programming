#include <stdio.h>

#include <fcntl.h>

#define BUF_SIZE 512

int main(int argc, char** argv)
{
  if(argc != 3)
  {
    fprintf(stderr, "usage: ./fwrite_copy <src_name> <dst_name>\n");
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

  /*
  * note that a failure at this fdopen() causes an empty dst_name file to be
  * created, some cleanup should be performed
  */
  FILE* dst_stream = fdopen(fd, "w");
  if(dst_stream == NULL)
  {
    perror("fopen");
    return 1;
  }

  char buf[BUF_SIZE];
  while(fread(buf, BUF_SIZE, 1, src_stream) == 1)
  {
    if(fwrite(buf, BUF_SIZE, 1, dst_stream) != 1)
    {
      perror("fwrite");
      return 1;
    }
  }

  if(ferror(src_stream))
  {
    perror("fread");
    return 1;
  }
  else
  {
    printf("received EOF\n");
  }

  if(fclose(src_stream) == EOF)
  {
    perror("fclose");
    return 1;
  }

  if(fclose(dst_stream) == EOF)
  {
    perror("fclose");
    return 1;
  }
}
