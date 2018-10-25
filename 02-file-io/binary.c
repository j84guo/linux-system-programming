#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct Person {
    int age;
    char name[100];
};

int main()
{
    struct Person p1 = {21, "this is p1"};
    char *wbuf = (char *) &p1;

    int fd = open("person.dat", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    int n = 0, size = sizeof p1, k;
    while (n < size) {
        k = write(fd, &wbuf + n, size - n);

        if (k == -1) {
            if (errno == EINTR)
                continue;

            perror("write");
            return 1;
        }

        n += k;
    }

    struct Person p2;
    lseek(fd, 0, SEEK_SET);
    
    /*
     * todo: handle partial read
     * bug: garbage data is read into the struct
     */
    if ((k = read(fd, &p2, size)) == -1) {
        perror("read");
        return 1;
    }

    close(fd);
    printf("%d\n", k);
    printf("%d\n", p2.age);

    return 0;
}
