#include <unistd.h>

/*
* see also
* int fdatasync(int fd);
* int fsync(int fd);
*/
int main()
{
  sync();
}
