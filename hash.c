#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "cell.h"
#include "global.h"

static unsigned hashf(const char *key)
{
  unsigned int value = 0;
  for (; *key != '\0'; key++)
    value = (value << 3) + *key;
  return value % HASH_SZ;
}

HASH_ELEM hash_tab[HASH_SZ];

cell_ptr hash(char *str)
{
  cell_ptr i;
  cell_ptr p = hashf(str);

#define search_loop(start, end) \
  for(i=start; i < end; i++) { \
    if((hash_tab[i].s!=NULL) && (strcmp(hash_tab[i].s, str) == 0)) \
      return i; \
    if(hash_tab[i].s == NULL) { \
      char *s = malloc(strlen(str)+1); \
      strcpy(s, str); \
      hash_tab[i].s = s; \
      return i; \
    }}

  search_loop(p, HASH_SZ);
  search_loop(0, p);

  longjmp(top, e_strtabof);
} 

void hash_usage()
{
  cell_ptr i;
  int n = 0;
  for(i=0; i<HASH_SZ; i++)
    if(hash_tab[i].s != NULL) n++;
  fprintf(stderr, "hash use ratio: %d/%d\n", n, HASH_SZ);
}
