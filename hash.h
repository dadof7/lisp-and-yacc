#define HASH_SZ 2003 // 素数

typedef struct {
  char* s;
} HASH_ELEM;

extern HASH_ELEM hash_tab[];

extern cell_ptr hash();
extern void hash_usage();
