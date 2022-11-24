#include <stdio.h>
#include <setjmp.h>

#include "cell.h"
#include "token.h"
#include "read.h"
#include "eval.h"
#include "print.h"
#include "global.h"

char *error_string;

cell_ptr top_level_env  = NIL;
cell_ptr the_last_expr  = NIL;
cell_ptr the_last_func  = NIL;
cell_ptr the_last_args  = NIL;

cell_ptr lambda_sym;
cell_ptr macro_sym;
cell_ptr progn_sym;
cell_ptr true_sym;
cell_ptr quote_sym;
cell_ptr bquote_sym;
cell_ptr comma_sym;
cell_ptr comma_at_sym;
cell_ptr andrest_sym;
cell_ptr debug_sym;

static void fun_time()
{
  cell_ptr p;

  for(p=FUNTAB_OFFSET; FUN_NM(p) != 0; p++) {
    top_level_env = cons(cons(mk_symcell(FUN_NM(p)), p), top_level_env);
  }
}

static void load_predef()
{
  set_token_buff("\
(progn\
  (setq t 't)\
  (setq lambda 'lambda)\
  (setq macro 'macro)\
  (setq 1+ '(lambda (n) (add2 1 n)))\
  (setq 1- '(lambda (n) (add2 -1 n)))\
  (setq list '(lambda (&rest l) l))\
  (setq mapcar '(lambda (f l) (unless l nil (cons (f (car l)) (mapcar f (cdr l))))))\
  'predef_OK\
)\
");

  print_sex( eval_sex(read_sex(get_token()), top_level_env) );
  printf("\n");
}

static void load_init(char* fname)
{
  FILE* f;

  if((f = fopen(fname, "r")) == 0)
    longjmp(top, e_fopenerr);

  set_token_fromf(f);
  while(1) {
    cell_ptr r = eval_sex( read_sex(get_token() ), top_level_env);
    print_sex(r);
    printf("\n");
  }
}

static void load_marker()
{
  top_level_env = cons( cons(mk_symcell("*** TOP LEVEL ENV BORDER ***"), true_sym), top_level_env);
}

void init()
{
  mark0();

  lambda_sym = mk_symcell("lambda");
  macro_sym =  mk_symcell("macro");
  true_sym = mk_symcell("t");
  progn_sym = mk_symcell("progn");
  quote_sym = mk_symcell("quote");
  bquote_sym = mk_symcell("_bquote");
  comma_sym = mk_symcell("_comma");
  comma_at_sym = mk_symcell("_comma_at");  
  andrest_sym = mk_symcell("&rest");
  debug_sym = mk_symcell("@debug");

  fun_time();
  load_marker();
  load_predef();
  load_init("init.l");
}
