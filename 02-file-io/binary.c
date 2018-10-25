#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct Person {
    int age;
    char name[100];
};

/*
 * todo:
 * handle partial write/read
 * garbage data is read into the struct when converting it to a char array
 */
int main()
{
    struct Person p1 = {21, "this is p1"};

    int fd = open("person.dat", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    int size = sizeof(struct Person);
    if (write(fd, &p1, size) == -1) {
        perror("write");
        return 1;
    }

    /*
     * don't check return since code is well-formed
     * we have a valid file descriptor, offset and no overflow occurs
     */
    lseek(fd, 0, SEEK_SET);
    struct Person p2 = {10, "test"};
    
    if (read(fd, &p2, size) == -1) {
        perror("read");
        return 1;
    }

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    printf("%d, %s\n", p2.age, p2.name);
    return 0;
}
