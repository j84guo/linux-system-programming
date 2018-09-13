#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>

struct Mode {
    mode_t bit;
    char *name;
};

struct Mode modes[] = {
    {S_IRUSR, "r"},
    {S_IWUSR, "w"},
    {S_IXUSR, "x"},
    {S_IRGRP, "r"},                                                             
    {S_IWGRP, "w"},                                                             
    {S_IXGRP, "x"},
    {S_IROTH, "r"},                                                             
    {S_IWOTH, "w"},                                                             
    {S_IXOTH, "x"}
};

int num_modes = sizeof(modes) / sizeof(struct Mode);

void print_mode(mode_t mode) {
    for(int i=0; i<num_modes; ++i) {
        char *s = (mode & modes[i].bit) ? modes[i].name : "-";
        printf("%s", s);
    }

    printf("\n");
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "usage: %s <path>\n", argv[0]);
        return 1;
    }

    DIR *dir;

    if((dir = opendir(argv[1])) == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *ent;
    struct stat ent_stat;
    int cap = 4;
    char *path = malloc(sizeof(char) * cap);

    while((ent = readdir(dir)) != NULL) {
        char *name = ent->d_name;
        int len = strlen(argv[1]) + strlen(name) + 1;

        while(len >= cap) {
            cap *= 2;
            path = realloc(path, sizeof(char) * cap);
        }

        strcpy(path, argv[1]);
        strcat(path, "/");
        strcat(path, name);

        if(stat(path, &ent_stat) == -1) {
            perror("stat");
            continue;
        }

        printf("%s ", path);
        printf(S_ISDIR(ent_stat.st_mode) ? "d" : "-");
        print_mode(ent_stat.st_mode);
    }

    free(path);
}
