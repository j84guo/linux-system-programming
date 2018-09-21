#include <unistd.h>
#include <stdio.h>

int main()
{
    /*
    * array of constant strings
    */
    char *const args[] = {"dep", "status", NULL};
    int ret = execv("/usr/local/bin/dep", args);
    if(ret == -1) perror("git");
}
