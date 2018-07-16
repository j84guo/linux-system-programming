#include <stdio.h>
#include <stdlib.h>

void malloc_int();
void malloc_rec();
void realloc_int();
void realloc_str();

struct rec
{
  int i;
  float f;
  char a;
};

int main()
{
  malloc_int();
  malloc_rec();
  realloc_int();
  realloc_str();

  return 0;
}

void malloc_int()
{
  int *p;

  if((p = (int*) malloc(sizeof(int))) == NULL)
  {
    fprintf(stderr, "malloc: out of memory");
    return;
  }

  *p = 10;

  printf("%d\n", *p);
  free(p);
}

void realloc_int()
{
  int *p = (int*) malloc(sizeof(int) * 2);

  *p = 10;
  *(p + 1) = 20;

  if((p = (int*) realloc(p, sizeof(int) * 3)) == NULL)
  {
    perror("realloc");
    return;
  }

  *(p + 2) = 30;

  for(int i=0; i<3; ++i)
    printf("%d\n", p[i]);

  free(p);
}

void realloc_str()
{
  char *s = (char*) malloc(sizeof(char) * 8);

  strcpy(s, "jackson");
  printf("String=%s, Address=%p\n", s, s);

  s = (char*) realloc(s, sizeof(char) * 17);

  strcat(s, " and ella");
  printf("String=%s, Address=%p\n", s, s);

  free(s);
}

void malloc_rec()
{
  struct rec *p;
  p = (struct rec*) malloc(sizeof(struct rec));

  p->i = 10;
  p->f = 2.5;
  p->a = 'a';

  printf("first value: %d\n", p->i);
  printf("second value: %f\n", p->f);
  printf("third value: %c\n", p->a);

  free(p);
}
