#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

#include "cell.h"
#include "token.h"
#include "print.h"
#include "eval.h"
#include "read.h"
#include "global.h"

jmp_buf top;

const char *mess[] =
  {
   "OK, setjmp saves the env, rady to jump back here in the future", /* e_ok = 0, */
   "need more tokens", /* e_eostr, */
   "bogus input", /* e_bogus, */
   "int expected", /* e_noint, */
   "cons cell expected", /* e_nocons, */
   "symbol table full", /* e_symtabof, */
   "string table full", /* e_strtabof, */
   "undefined", /* e_undef, */
   "apply failed", /* e_apply, */
   "no more cons cells", /* e_consof, */
   "div by 0", /* e_div0, */
   "wrong number of arguments", /* e_arg, */
   "wrong types of arguments", /* e_argtype, */
   "symbol expected", /* e_notsym, */
   "tokenp", /* e_tokenp, */
   "end of file", /* e_eof, */
   "mulformed arguments", /* e_mularg, */
   "parser error", /* e_parseerr, */
   "duplicated formal arguments", /* e_dupargs, */
   "file error", /* e_fopenerr, */
   "unix exception", /* e_unix_exception */
   ",@ must appear inside of a list", /* e_commaat_by_itself */
   ",@ expecting a form of list", /* e_commaat_evaled_to_non_list */
   "list was expected", // e_notlist
   "macro expanded", // e_macroex
   "eval error", // e_evalerr
};

void report_err(int err) 
{
  switch(err) {
  case e_ok:
    break;
  case e_undef:
    fprintf(stderr, "*** %s: ", mess[err]);
    fprintf(stderr, "'%s' ***\n", error_string);
    goto short_info;
  case e_apply:
  case e_nocons:
  case e_noint:
  case e_notsym:
  case e_argtype:
  case e_arg:
  case e_div0:
  case e_mularg:
  case e_dupargs:
    fprintf(stderr, "*** %s ***\n", mess[err]);
    goto info;
  default:
    fprintf(stderr, "*** %s ***\n", mess[err]);
    goto short_info;
  }
  
  if(0) { // do not execute this block, goto entries only
  info:
    printf("func: ");
    print_sex(the_last_func);
    printf("\n");
 
    printf("args: ");
    print_sex(the_last_args);
    printf("\n");
  short_info:
    printf("---\n");
    cns_usage();
  }
}

void unix_exception(int signal)
{
  longjmp(top, e_unix_exception);
}

int main()
{
  int err;
  
  printf(";; *** Tiny Lisp ***\n");
  printf(";; small enogh to compile with your program,\n");
  printf(";; inspired by the work of Yuumi Yoshida at ey-office.com\n");
  /* printf(";;       int: %ld-bit,\n", sizeof(int)*8); */
  /* printf(";;      long: %ld-bit,\n", sizeof(long)*8); */
  /* printf(";; long long: %ld-bit,\n", sizeof(long long)*8); */
  /* printf(";;   pointer: %ld-bit,\n", sizeof(cns_cell*)*8); */
  printf(";; your cons cell: %ld-byte.\n", sizeof(cns_cell));
  printf("\n");

  if((err = setjmp(top)) == e_ok) { // longjmp comes back here while initialization
    init();
  }
  else {
    report_err(err);
  }

  set_token_fromf(stdin);

 retry:
  if((err = setjmp(top)) == e_ok) { // longjmp comes back here
    signal(SIGINT, unix_exception);
    signal(SIGFPE, unix_exception);
      
    // REPL
    while(1) { 
      if(is_cnstab_tight() || gc_flag) {
	fflush(stdout);
	gc(); // gc is safe iff called at the top level
      }

      printf("> ");

      the_last_expr = eval_sex( read_sex(get_token() ), top_level_env);

      print_sex(the_last_expr);
      printf("\n");
    } // REPL
  }
  else {
    report_err(err);
    if(err == e_eof) return 0;
    else goto retry;
  }
}
