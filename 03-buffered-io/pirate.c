#include <stdio.h>

struct Pirate
{
  char name[100];
  unsigned long booty;
  unsigned int beard_len;
};

int main()
{
  struct Pirate p;
  struct Pirate blackbeard = {"Edward Teach", 950, 48};

  FILE *out = fopen("pirate_data", "w");
  if(out == NULL)
  {
    perror("fopen");
    return 1;
  }

  if(fwrite(&blackbeard, sizeof blackbeard, 1, out) != 1)
  {
    perror("fwrite");
    return 1;
  }

  if(fclose(out) == EOF)
  {
    perror("fclose");
    return 1;
  }

  FILE *in = fopen("pirate_data", "r");
  if(in == NULL)
  {
    perror("fopen");
    return 1;
  }

  if(fread(&p, sizeof p, 1, in) != 1)
  {
    /*
    * here we know that there should be at least one block of data of size
    * struct Pirate in the file, since we just wrote to it, therefore there is
    * no need to check feof()
    */
    perror("fread");
    return 1;
  }

  if(fclose(out) == EOF)
  {
    perror("fclose");
    return 1;
  }

  printf("Read a struct Pirate: %s, %lu, %d\n", p.name, p.booty, p.beard_len);
}
