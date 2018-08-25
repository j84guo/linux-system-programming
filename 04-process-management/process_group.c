#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){
    pid_t pgid = getpgid(0);

    printf("%d\n", pgid);
}
