#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <alloca.h>

#include "cell.h"
#include "global.h"
#include "eval.h"
#include "print.h"
#include "y.tab.h"

#define BIND(x,args) \
  if(IS_NIL(args))longjmp(top,e_arg);x=CAR(args);args=CDR(args)
#define EBIND(x,args,env) BIND(x,args);x=eval_sex(x,env)
#define ARE_INTS(p1,p2) (IS_INT(p1) && IS_INT(p2))

cell_ptr _progn(cell_ptr args, cell_ptr env) {
  cell_ptr a = NIL;
  while(! ENDP(args)) {
    BIND(a,args);
    // don't evaluate the last arg
    if(IS_NIL(args)) return a;
    a = eval_sex(a, env);
  }
  return a;
}

cell_ptr _cond(cell_ptr args, cell_ptr env) {
  while(! IS_NIL(args)) {
    cell_ptr clause = CAR(args);

    if(! IS_NIL(clause)) {
      if(! IS_CNS(clause)) longjmp(top, e_argtype);

      cell_ptr cond_part = CAR(clause);
      cell_ptr b = eval_sex(cond_part, env);
      if(! IS_NIL(b)) {
	if(IS_NIL(CDR(clause))) return cons(quote_sym, cons(b, NIL)); // avoid evaluation in eval_sex
	return _progn(CDR(clause), env);
      }
    }
    args = CDR(args);
  }
  return NIL;
}

cell_ptr _tostr(cell_ptr args, cell_ptr env) {
  cell_ptr a;
  char buff[200];
  EBIND(a, args, env);
  if(IS_STR(a)) {
    return a;
  }
  else if(IS_INT(a)) {
    sprintf(buff, INT_FORMAT, CELL2INT(a));
    return mk_strcell(buff);
  }
  else if(IS_REAL(a)) {
    sprintf(buff, REAL_FORMAT, CELL2REAL(a));
    return mk_strcell(buff);
  }
  else
    longjmp(top, e_argtype);
}

cell_ptr _gensym(cell_ptr args, cell_ptr env) {
  static int n = 0;
  char buff[200];
  sprintf(buff, "#GENSYM-%05d", n++);
  return mk_symcell(buff);
}

cell_ptr _concat(cell_ptr args, cell_ptr env) {
  cell_ptr a, b;
  EBIND(a, args, env);
  EBIND(b, args, env);
  if(IS_STR(a) && IS_STR(b)) {
    int sizea = strlen(STR_STR(a))+1;
    int sizeb = strlen(STR_STR(b))+1;
    char* buff = alloca(sizea+sizeb);
    sprintf(buff, "%s%s", STR_STR(a), STR_STR(b));
    return mk_strcell(buff);
  }
  else
    longjmp(top, e_argtype);
}

CLJ _clet(cell_ptr args, cell_ptr env) {
  cell_ptr bind_part, new_env = env;
  BIND(bind_part,args);
  while(! IS_NIL(bind_part)) {
    cell_ptr bind = CAR(bind_part);
    cell_ptr s = CAR(bind);
    cell_ptr x = CAR(CDR(bind));
    new_env = cons( cons(s, eval_sex(x, env)), new_env);
    bind_part = CDR(bind_part);
  }
  CLJ clj;
  clj.expr = _progn(args, new_env);
  clj.env = new_env;
  return clj;
}


CLJ _clet_star(cell_ptr args, cell_ptr env) {
  cell_ptr bind_part, new_env = env;
  BIND(bind_part,args);
  while(! IS_NIL(bind_part)) {
    cell_ptr bind = CAR(bind_part);
    cell_ptr s = CAR(bind);
    cell_ptr x = CAR(CDR(bind));
    new_env = cons( cons(s, eval_sex(x, new_env)), new_env);
    bind_part = CDR(bind_part);
  }
  CLJ clj;
  clj.expr = _progn(args, new_env);
  clj.env = new_env;
  return clj;
}

typedef struct {
  int k;
  /* -1: error
      0: both int
      1: both real (may be coerced to real)
   */
  union {
    struct { INT_TYPE iv1, iv2; };
    struct { REAL_TYPE rv1, rv2; };
  };
} BALANCE;

// a1とa2を見て、どちらのタイプに寄せればいいか考える
void balance(cell_ptr a1, cell_ptr a2, BALANCE *b) {
  if(IS_NUM(a1) && IS_NUM(a2)) {
    if(ARE_INTS(a1, a2)) {
      b->iv1 = CELL2INT(a1);
      b->iv2 = CELL2INT(a2);
      b->k = 0;
      return;
    }
    b->rv1 = IS_INT(a1)? (REAL_TYPE) CELL2INT(a1): CELL2REAL(a1);
    b->rv2 = IS_INT(a2)? (REAL_TYPE) CELL2INT(a2): CELL2REAL(a2);
    b->k = 1;
    return;
  }
  b->k = -1;
}

#define MOD2(a,b) (((INT_TYPE)(a))*((INT_TYPE)(b)))
#define MUL2(a,b) ((a)*(b))
#define SUB2(a,b) ((a)-(b))
#define ADD2(a,b) ((a)+(b))
#define DIV2(a,b) ((a)/(b))

#define BINOP(f,op) \
cell_ptr f(cell_ptr args, cell_ptr env) { \
  cell_ptr p1,p2; \
  EBIND(p1, args, env); \
  EBIND(p2, args, env); \
  BALANCE b; \
  balance(p1, p2, &b); \
  if(b.k == 0) return INT2CELL(op(b.iv1, b.iv2)); \
  if(b.k == 1) return REAL2CELL(op(b.rv1, b.rv2)); \
  longjmp(top, e_argtype); }

BINOP(_mod,MOD2);
BINOP(_mul,MUL2);
BINOP(_sub,SUB2);
BINOP(_add,ADD2);
BINOP(_div,DIV2);

#define CMP(f,op) \
cell_ptr f(cell_ptr args,cell_ptr env) { \
  cell_ptr p1,p2; \
  EBIND(p1,args,env); \
  EBIND(p2,args,env); \
  BALANCE b; \
  balance(p1,p2,&b); \
  if(b.k == 0) { \
    if(b.iv1 op b.iv2) return true_sym; \
    else return NIL; \
  } \
  if(b.k == 1) { \
    if(b.rv1 op b.rv2) return true_sym; \
    else return NIL; \
  } \
  longjmp(top, e_argtype); }
  
CMP(_gt,>)
CMP(_gte,>=)
CMP(_ls,<)
CMP(_lse,<=)

cell_ptr _not(cell_ptr args, cell_ptr env) {
  cell_ptr b;
  EBIND(b, args, env);
  if(IS_NIL(b)) return true_sym;
  else return NIL;
}

cell_ptr _if(cell_ptr args, cell_ptr env) {
  cell_ptr b,x;
  EBIND(b, args, env);

  // 同時に２つの式を評価しないのがポイント
  if (b != NIL) { // true
    BIND(x, args);
    return x;
  }
  else { // false
    args = CDR(args);  // skip an expr for true
    while(! IS_NIL(args)) {
      BIND(x, args);
      if(IS_NIL(args)) return x;
      eval_sex(x, env);
    }
    return NIL;
  }
}


cell_ptr _setq(cell_ptr args, cell_ptr env) {
  cell_ptr symbol;
  BIND(symbol, args);
  // printf("setq:");
  // print_sex(symbol);
  // printf("¥n");

  if(IS_SYM(symbol)) {
    cell_ptr val, pair = assoc(symbol, env);
    EBIND(val, args, env);

    if(IS_NIL(pair)) { // not found, insert into the top
      cell_ptr entry = cons(cons(symbol, val), NIL);
      cell_ptr xcdr = CDR(top_level_env);
      CDR(top_level_env) = entry;
      CDR(entry) = xcdr;
    }
    else CDR(pair) = val;
    return val;
  }
  longjmp(top, e_notsym);
}

cell_ptr _quote(cell_ptr args, cell_ptr env) {
  cell_ptr p;
  BIND(p,args);
  if(!IS_NIL(args))longjmp(top,e_arg);  
  return p;
}

static cell_ptr copy_list(l)
{
  if(! IS_CNS(l)) return l;
  return cons(CAR(l), copy_list(CDR(l)));
}

cell_ptr _bq_build(cell_ptr a, cell_ptr env, int nest)
{
  // expr -> NON-CONS-OBJECT
  //      -> (_comma  expr )
  //      -> ( expr+ )
  if(!IS_CNS(a))
    return a;

  cell_ptr fnm = CAR(a);
  if(fnm==comma_sym || fnm==comma_at_sym) {
    if(nest==0 && fnm==comma_at_sym) longjmp(top,e_commaat_by_itself);
    
    cell_ptr b = CAR(CDR(a));
    return eval_sex(b, env);
  }

  cell_ptr head, tail;
  head = tail = NIL;
  while(IS_CNS(a)) {
    cell_ptr next_expr = CAR(a); /* next expr */
    cell_ptr b; // 評価後のnext_expr

    // 2重のバッククオートの評価しない
    if(IS_CNS(next_expr) && CAR(next_expr)==bquote_sym)
      b = next_expr;
    else
      b = _bq_build(next_expr, env, nest+1);

    // もしnext_exprが(comma_at ...)でかつそれを評価してリストが返ってきたら
    // まずそのリストをコピー(破壊的に変更するので)、その後コピーの方を
    // tailにくっつけて、tailを最後まで移動
    cell_ptr nb;
    if(IS_CNS(next_expr) && CAR(next_expr)==comma_at_sym) {
      if(!IS_CNS(b) && !IS_NIL(b)) longjmp(top, e_commaat_evaled_to_non_list);
      nb = copy_list(b);
      if(IS_NIL(head)) head = tail = nb;
      else CDR(tail) = nb;
      while(IS_CNS(CDR(tail))) { // tailをnbの最後まで移動
	tail = CDR(tail);
      }
    }
    else { // (comma_at...じゃなかった
      nb = cons(b, NIL);
      if(IS_NIL(head)) head = tail = nb;
      else CDR(tail) = nb;
      tail = nb;
    }
    a = CDR(a);
  }
  return head;
}

cell_ptr _bquote(cell_ptr args, cell_ptr env) {
  cell_ptr a;
  BIND(a,args);
  if(!IS_NIL(args)) longjmp(top,e_arg);
  return _bq_build(a, env, 0);
}

cell_ptr _pr(cell_ptr args, cell_ptr env) {
  cell_ptr p;
  while(!IS_NIL(args)) {
    EBIND(p, args, env);
    if(IS_STR(p))
      printf("%s", STR_STR(p));
    else
      print_sex(p);
    if(!IS_NIL(args)) printf(" ");
  }
  return NIL;
}

cell_ptr _prln(cell_ptr args, cell_ptr env) {
  cell_ptr result = _pr(args, env);
  printf("\n");
  return result;
}

cell_ptr _dump(cell_ptr args, cell_ptr env) {
  printf("env:");
  print_sex(top_level_env);
  printf("\n");
  cns_usage();
  return NIL;
}

extern int cons_counter;

cell_ptr _cons_counter(cell_ptr args, cell_ptr env) {
  printf("cons called since the last gc: %d times\n", cons_counter);
  return NIL;
}

cell_ptr _gc(cell_ptr args, cell_ptr env) {
  gc_flag = 1;
  return NIL;
}

cell_ptr _cons(cell_ptr args, cell_ptr env) {
  cell_ptr p1,p2;
  EBIND(p1, args, env);
  EBIND(p2, args, env);
  return cons(p1, p2);
}

cell_ptr _env(cell_ptr args, cell_ptr env) {
  return env; // top_level_env;
}

cell_ptr _car(cell_ptr args, cell_ptr env) {
  cell_ptr p;
  EBIND(p, args, env);
  if(IS_NIL(p)) return NIL;
  else if(IS_CNS(p)) return CAR(p);
  longjmp(top, e_nocons);
}

cell_ptr _cdr(cell_ptr args, cell_ptr env) {
  cell_ptr p;
  EBIND(p, args, env);
  if(IS_NIL(p)) return NIL;
  if(IS_CNS(p)) return CDR(p);
  longjmp(top, e_nocons);
}

cell_ptr _cdrx(cell_ptr args, cell_ptr env) {
  cell_ptr p, v;
  EBIND(p, args, env);
  if(IS_CNS(p)) {
    EBIND(v, args, env);
    return CDR(p) = v;
  }
  longjmp(top, e_nocons);
}

cell_ptr __eq(cell_ptr a1, cell_ptr a2) {
  if(IS_NIL(a1) && IS_NIL(a2)) {
    return true_sym;
  }
  else if(IS_NUM(a1) && IS_NUM(a2)) {
    if(ARE_INTS(a1, a2) && CELL2INT(a1)==CELL2INT(a2)) return true_sym;
    REAL_TYPE v1 = IS_INT(a1)? (REAL_TYPE) CELL2INT(a1): CELL2REAL(a1);
    REAL_TYPE v2 = IS_INT(a2)? (REAL_TYPE) CELL2INT(a2): CELL2REAL(a2);
    if(v1 == v2) return true_sym;
    return NIL;
  }
  else if(a1==a2) {
    return true_sym;
  }
  return NIL;
}

cell_ptr _eq(cell_ptr args, cell_ptr env) {
  cell_ptr a1,a2;
  EBIND(a1, args, env);
  EBIND(a2, args, env);
  return __eq(a1, a2);
}

cell_ptr _assoc(cell_ptr args, cell_ptr env) {
  cell_ptr a, l, pair;
  EBIND(a, args, env);
  EBIND(l, args, env);

  while(IS_CNS(l)) {
    pair = CAR(l);
    if(IS_CNS(pair) && __eq(a, CAR(pair))) return pair;
    l = CDR(l);
  }
  return NIL;
}

cell_ptr _foreach(cell_ptr args, cell_ptr env) {
  // (foreach i l exprs)
  cell_ptr i, l;
  BIND(i, args);
  if(! IS_SYM(i)) longjmp(top, e_notsym);
  
  EBIND(l, args, env);
  if(! IS_CNS(l)) longjmp(top, e_nocons);

  cell_ptr head, tail;
  head = tail = NIL;
  while(IS_CNS(l)) {
    cell_ptr a = CAR(l);
    cell_ptr env2 = cons(cons(i, a), env);
    cell_ptr v = eval_sex(cons(progn_sym, args), env2);
    if(IS_NIL(head)) {
      head = tail = cons(v, NIL);
    }
    else {
      cell_ptr b = CDR(tail) = cons(v, NIL);
      tail = b;
    }
    l = CDR(l);
  }
  if(! IS_NIL(l)) longjmp(top, e_notlist);
  
  return head;
}


cell_ptr _foreach_t(cell_ptr args, cell_ptr env) {
  // (foreach i l exprs)
  cell_ptr i, l;
  BIND(i, args);
  if(! IS_SYM(i)) longjmp(top, e_notsym);
  
  EBIND(l, args, env);
  if(! IS_CNS(l)) longjmp(top, e_nocons);

  cell_ptr head, tail;
  head = tail = NIL;
  while(IS_CNS(l)) {
    cell_ptr a = CAR(l);
    cell_ptr env2 = cons(cons(i, a), env);
    cell_ptr v = eval_sex(cons(progn_sym, args), env2);
    if(! IS_NIL(v)) { // nil is skipped
      if(IS_NIL(head)) {
	head = tail = cons(v, NIL);
      }
      else {
	cell_ptr b = CDR(tail) = cons(v, NIL);
	tail = b;
      }
    }
    l = CDR(l);
  }
  if(! IS_NIL(l)) longjmp(top, e_notlist);
  
  return head;
}


cell_ptr _atom(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_NIL(a1) || IS_INT(a1) || IS_REAL(a1) || IS_SYM(a1) || IS_STR(a1)) return true_sym;
  return NIL;
}

cell_ptr _len(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_STR(a1)) return mk_intcell((INT_TYPE) strlen(STR_STR(a1)));
  longjmp(top, e_argtype);
}

cell_ptr _consp(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_CNS(a1)) return true_sym;
  return NIL;
}

cell_ptr _intp(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_INT(a1)) return true_sym;
  return NIL;
}

cell_ptr _nump(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_NUM(a1)) return true_sym;
  return NIL;
}

cell_ptr _null(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_NIL(a1)) return true_sym;
  return NIL;
}

cell_ptr _endp(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(ENDP(a1)) return true_sym;
  return NIL;
}

cell_ptr _listp(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_NIL(a1) || IS_CNS(a1)) return true_sym;
  return NIL;
}

cell_ptr _and(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  while(! IS_NIL(args)) {
    EBIND(a1, args, env);
    if(IS_NIL(a1)) return NIL;
  }
  return true_sym;
}

cell_ptr _or(cell_ptr args, cell_ptr env) {
  cell_ptr a1;
  while(! IS_NIL(args)) {
    EBIND(a1, args, env);
    if(! IS_NIL(a1)) return true_sym;
  }
  return NIL;
}

extern FILE *yyin;
extern int yylineno;
extern void reset_flex();
extern int yyparse();

cell_ptr parse_result;

cell_ptr _parse(cell_ptr args, cell_ptr env)
{
  cell_ptr a1;
  EBIND(a1, args, env);
  if(!IS_STR(a1)) longjmp(top, e_argtype);

  yyin = fopen(STR_STR(a1), "r");
  if(yyin == NULL) longjmp(top, e_fopenerr);

  yylineno = 1;
  parse_result = NIL;
  reset_flex();
  yyparse();
  fclose(yyin);
  return parse_result;
}

cell_ptr _range(cell_ptr args, cell_ptr env) {
  cell_ptr a, b, head;
  EBIND(a, args, env);
  EBIND(b, args, env);  
  if(IS_INT(a) && IS_INT(b)) {
    INT_TYPE aa = CELL2INT(a);
    INT_TYPE bb = CELL2INT(b);
    head = NIL;
    for(bb=bb-1; aa <= bb; bb--) {
      head = cons(mk_intcell(bb), head);
    }
    return head;
  }
  return NIL;
}

cell_ptr _symbolp(cell_ptr args, cell_ptr env)
{
  cell_ptr a1;
  EBIND(a1, args, env);
  if(IS_SYM(a1) || IS_NIL(a1)) return true_sym;
  return NIL;
}

cell_ptr _colon_symbolp(cell_ptr args, cell_ptr env)
{
  cell_ptr a1;
  EBIND(a1, args, env);
  if(!IS_SYM(a1)) return NIL;
  if(SYM_NM(a1)[0] == ':') return true_sym;
  return NIL;
}


cell_ptr _intern(cell_ptr args, cell_ptr env)
{
  cell_ptr a1;
  EBIND(a1, args, env);
  if(!IS_STR(a1)) longjmp(top, e_argtype);
  return mk_symcell(STR_STR(a1));
}


cell_ptr _eval(cell_ptr args, cell_ptr env)
{
  cell_ptr a1;
  EBIND(a1, args, env);
  return eval_sex(a1, env);
}


cell_ptr _macroexpand(cell_ptr args, cell_ptr env)
{
  cell_ptr a1, env2;
  EBIND(a1, args, env);
  env2 = cons(cons(debug_sym, true_sym), env);
  return eval_sex(a1, env2);
}

cell_ptr _funcall(cell_ptr args, cell_ptr env)
{
  cell_ptr fn, a;
  EBIND(fn, args, env);
  cell_ptr head, tail;
  head = tail = NIL;
  while(! IS_NIL(args)) {
    EBIND(a, args, env);    
    if(head == NIL)
      head = tail = cons(a, NIL);
    else {
      cell_ptr b = cons(a, NIL);
      CDR(tail) = b;
      tail = b;
    }
  }
  CLJ clj = apply(fn, head, env, 1);
  return eval_sex(clj.expr, clj.env);
}

cell_ptr _apply(cell_ptr args, cell_ptr env)
{
  cell_ptr fn, a;
  EBIND(fn, args, env);
  cell_ptr head, tail;
  head = tail = NIL;
  while(! IS_NIL(args)) {
    EBIND(a, args, env);
    if(head == NIL) {
      if(IS_NIL(args)) head = a; // 最後の引数なら, a must be a list or nil
      else head = tail = cons(a, NIL);
    }
    else {
      if(IS_NIL(args)) CDR(tail) = a; // 最後の引数なら, a must be a list or nil
      else {
	cell_ptr b = cons(a, NIL);
	CDR(tail) = b;
	tail = b;
      }
    }
  }
  CLJ clj = apply(fn, head, env, 1); // 1: do not eval args
  return eval_sex(clj.expr, clj.env);
}



fun_cell funtab[] = 
{
  { ">", &_gt },
  { ">=", &_gte },
  { "<", &_ls },
  { "<=", &_lse },
  { "%", &_mod },
  { "mul2", &_mul },
  { "sub2", &_sub },
  { "add2", &_add },
  { "div2", &_div },
  { "if", &_if, 1 },
  { "not", &_not },
  { "setq", &_setq },
  { "quote", &_quote },
  { "_bquote", &_bquote },
  { "dump", &_dump },
  { "gc", &_gc }, 
  { "cons", &_cons },
  { "env", &_env },
  { "car", &_car },
  { "cdr", &_cdr },
  { "cdr!", &_cdrx },  
  { "=", &_eq },
  { "eq", &_eq },
  { "atom", &_atom },
  { "intp", &_intp },
  { "nump", &_nump },
  { "symbolp", &_symbolp },
  { "symbolp:", &_colon_symbolp },
  { "listp", &_listp },
  { "consp", &_consp },
  { "null", &_null },
  { "endp", &_endp },  
  { "and", &_and },
  { "or", &_or },
  { "progn", &_progn, 1 },
  { "let", .fp2 = &_clet, 2 },
  { "let*", .fp2 = &_clet_star, 2 },  
  { "parse", &_parse },
  { "intern", &_intern },  
  { "cond", &_cond, 1 },
  { "concatenate", &_concat },
  { "cons-counter", &_cons_counter },
  { "pr", &_pr },  
  { "prln", &_prln },
  { "len", &_len },
  { "assoc", &_assoc },
  { "2str", &_tostr },
  { "gensym", &_gensym },
  { "macroexpand", &_macroexpand },
  { "funcall", &_funcall },
  { "apply", &_apply },
  { "eval", &_eval },
  { "range", &_range },
  { "foreach", &_foreach },
  { "foreach-t", &_foreach_t },
  { (void*)0, (void*)0 },
};
