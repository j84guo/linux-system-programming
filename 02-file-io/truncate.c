#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

int main()
{
  char* name = "pirate.txt";
  char* text = "Edward Teach was a notorious English pirate.\nHe was nicknamed Blackbeard.\n";

  int fd = open(name, O_WRONLY | O_CREAT | O_EXCL, 0644);
  if(fd == -1)
  {
    perror("open");
    return 1;
  }

  ssize_t nw = write(fd, text, strlen(text));
  if(nw == -1)
  {
    perror("write");
    return 1;
  }

  int rc = truncate(name, 45);
  if(rc == -1)
  {
    perror("truncate");
    return 1;
  }

  rc = ftruncate(fd, 74);
  if(rc == -1)
  {
    perror("ftruncate");
    return 1;
  }

  off_t pos = lseek(fd, -8, SEEK_END);
  if(pos == -1)
  {
    perror("lseek()");
    return 1;
  }

  nw = write(fd, "end here", 8);
  if(nw == -1)
  {
    perror("write");
    return 1;
  }

  return 0;
}
