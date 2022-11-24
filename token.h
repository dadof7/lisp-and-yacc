#define TOKEN_STR_SZ (1024 * 2) 

typedef struct {
  char kind;
  union {
    long lval;
    double dval;
    char str[TOKEN_STR_SZ+1];
  };
} token_t;

extern void set_token_buff(char*);
extern void set_token_fromf(FILE*);
extern token_t* get_token();

