#include <stdio.h>
#include <unistd.h>

int main()
{
    // note the first argument is the name of the program
    int ret = execl("/usr/bin/vim", "vim", "notes.txt", NULL);

    if(ret == -1)   
        perror("execl");
}
