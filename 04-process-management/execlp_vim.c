#include <unistd.h>
#include <stdio.h>

int main()
{
    int ret = execlp("vim", "vim", "execlp_vim.c", NULL);
    if(ret == -1) perror("execlp");
}
