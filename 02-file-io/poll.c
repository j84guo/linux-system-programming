#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#define TIMEOUT 5

int main(void)
{
  struct pollfd fds[2];
  int ret;

  // watch stdin for input
  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;

  // watch stdout for output
  fds[1].fd = STDOUT_FILENO;
  fds[1].events = POLLOUT;

  // block
  if((ret = poll(fds, 2, TIMEOUT * 1000)) == -1)
  {
    perror("poll");
    return 1;
  }

  // no fd's ready
  if(ret == 0)
  {
    printf("%d seconds elapsed.\n", TIMEOUT);
    return 0;
  }

  // check pollfd structures for readiness
  if(fds[0].revents & POLLIN)
    printf("stdin is readable\n");

  if(fds[1].revents & POLLOUT)
    printf("stdout is writable\n");

  return 0;
}
