#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define READ_SIZE 512
#define OUT_FILE "kmp_out.txt"

/*
* text buffer and its size
*/
struct StringBuffer
{
  char* text;
  int length;
};

/*
* reads a file as a dynamically allocated C string
*/
int read_string_buffer(char* path, struct StringBuffer *sb);

/*
* frees the buffer associated with a StringBuffer structure
*/
void free_string_buffer(struct StringBuffer *sb);

/*
* prints usage text
*/
void print_usage();

/*
* kmp prefix table
*/
void build_prefix_table(int* A, char* p, int m);

/*
* finds pattern in a string buffer
*/
void find(struct StringBuffer *sb, char *pattern);

/*
* runs kmp search on text and pattern
*/
void kmp_search(int* prefix_table, char* pattern, int m, struct StringBuffer* sb);

/*
* writes all bytes in buffer to file descriptor until error
*/
int write_all(int fd, char* buf, size_t len);

int main(int argc, char **argv)
{
  if(argc != 3)
  {
    print_usage();
    return 1;
  }

  struct StringBuffer sb;
  if(read_string_buffer(argv[1], &sb) == -1)
  {
    perror("read_string_buffer");
    return 1;
  }

  find(&sb, argv[2]);
  free_string_buffer(&sb);

  return 0;
}

void find(struct StringBuffer *sb, char *pattern)
{
  int m = strlen(pattern);
  if(m > sb->length)
    return;

  int prefix_table[m + 1];

  build_prefix_table(prefix_table, pattern, m);
  kmp_search(prefix_table, pattern, m, sb);
}

void kmp_search(int* A, char* pattern, int m, struct StringBuffer* sb)
{
  int fd = open(OUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR |
                S_IRGRP | S_IROTH);

  int i = 0;
  int j = 0;

  for(i = 0; i<sb->length; ++i){
    while(j != -1 && sb->text[i] != pattern[j])
      j = A[j];

    if(++j == m)
    {
      if(write_all(fd, "found match\n", 12) == -1)
      {
        perror("write_all");
        return;
      }

      j = A[m];
    }
  }

  if(close(fd))
    perror("close");
}

void build_prefix_table(int* A, char* p, int m)
{
  int j = -1;
  int i = 0;

  A[i] = j;

  for(i = 0; i<m; ++i){
    while(j != -1 && p[i] != p[j])
      j = A[j];

    A[i+1] = ++j;
  }

  for(int i=0; i<m+1; ++i)
    printf("%d ", A[i]);

  printf("\n");
}

void print_usage()
{
  printf("usage: ./kmp <file> <pattern>\n");
}

int read_string_buffer(char* path, struct StringBuffer *sb)
{
  int fd = open(path, O_RDONLY);
  if(fd == -1)
    return -1;

  int capacity = READ_SIZE;
  int n = 0;
  int r = 0;

  sb->text = (char*) malloc(sizeof(char) * (capacity + 1));

  while((r = read(fd, sb->text + n, READ_SIZE)) != 0)
  {
    if(r == -1)
    {
      if(r == EINTR)
        continue;

      return -1;
    }

    n += r;
    sb->text[n] = '\0';

    if(capacity < n + READ_SIZE)
    {
      capacity *= 2;
      sb->text = (char*) realloc(sb->text, sizeof(char) * (capacity + 1));
    }
  }

  sb->length = n;

  if(close(fd))
    return -1;

  return 0;
}

void free_string_buffer(struct StringBuffer *sb)
{
  free(sb->text);
}

int write_all(int fd, char* buf, size_t len)
{
  size_t i = 0;
  ssize_t nw;

  while(len > 0)
  {
    nw = write(fd, buf + i, len);
    if(nw == -1)
      return -1;

    len -= nw;
    i += nw;
  }

  return 0;
}
