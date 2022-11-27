#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include "cell.h"
#include "global.h"
#include "print.h" // for degug
#include "eval.h"

cell_ptr assoc(cell_ptr s, cell_ptr env) {
  // search list of dot-pairs, and return a dot-pair
  cell_ptr pair;
  while(! IS_NIL(env)) {
    pair = CAR(env);
    if(CAR(pair) == s) return pair;
    env = CDR(env);
  }
  return NIL;
}

static cell_ptr destructive_append(cell_ptr list1, cell_ptr list2) 
{
  // *** DESTRUCTIVE **
  // destroy the last cell of list1
  if(IS_NIL(list1)) return list2;

  cell_ptr p = list1;
  while(! IS_NIL(CDR(p))) p = CDR(p); // find the end
  CDR(p) = list2;
  return list1;
}

static cell_ptr eval_list(cell_ptr elist, cell_ptr env) {
  if(IS_NIL(elist))
    return NIL;
  else
    return cons(eval_sex(CAR(elist), env), eval_list(CDR(elist), env));
}

static cell_ptr pair_list(cell_ptr formals, cell_ptr args)
{
  // (f1 f2 f3...) (a1 a2 a3...) => ((f1.a1) (f2.a2) (f3.a3)...)
  if(IS_NIL(formals)) return NIL; // no more formal parameters

  cell_ptr s = CAR(formals);
  cell_ptr v;

  // &rest
  if(s == andrest_sym) {
    s = CDR(formals);
    if(IS_NIL(s)) return NIL; // no formal param after &rest; cannot bind further
    s = CAR(s);
    v = args;
    return cons(cons(s, v), NIL);
  }

  if(IS_NIL(args)) {
    // needs more actual params
    longjmp(top, e_arg);
  }

  cell_ptr tmpenv = pair_list(CDR(formals), CDR(args));
  
  if(! IS_NIL(assoc(s, tmpenv))) {
    // same formal parameters declared in this environment more than onece
    longjmp(top, e_dupargs);
  }

  v = CAR(args);
  return  cons(cons(s, v), tmpenv);
}

cell_ptr eval_sex(cell_ptr e, cell_ptr env) {
 EVAL_TOP:
  
  // if e is an atom return as is
  if (IS_NIL(e) || IS_INT(e) || IS_REAL(e) || IS_STR(e)) return e;
 
  // if e is a symbol => variable, evaluates e in env
  if (IS_SYM(e)) {
    if(SYM_NM(e)[0] == ':') // symbol starting : evaluates to itself
      return e;
    
    cell_ptr v = assoc(e, env);
    if (! IS_NIL(v)) return CDR(v);
    
    error_string = SYM_NM(e);
    longjmp(top, e_undef);
  }
 
  // (lambda ...) evaluates to itself
  cell_ptr func = CAR(e);
  if(func == lambda_sym)
    return e; 
 
  // e is a list => func, macro, special-form
  // func must be evaluated as a form of: (lambda...), (macro...) or native
  // args is a list of actual arguments; they are evaluated and bound to formal params
  cell_ptr args = CDR(e);
  the_last_func = func;
  the_last_args = args;
 
  // this check is needed, since macro might create strange expressions
  if(!IS_NIL(args) && !IS_CNS(args)) {
    longjmp(top, e_mularg);
  }
 
  // fbody is a form of (lambda...), (macro...), or pointer to a native form(function/special-form)
  cell_ptr fbody = eval_sex(func, env);
  
  if (IS_FUN(fbody)) { // native forms decide how to cook their args
    switch(FUN_TF(fbody)) {
    CLJ clj;
    case 0: // normal native form
      return (*FUN_FP(fbody))(args, env);      
    case 1: // tc native form
      e = (*FUN_FP(fbody))(args, env);
      goto EVAL_TOP;
    case 2: // tc affecting env native form (let, let*)
      clj = (*FUN_FP2(fbody))(args, env);
      e = clj.expr;
      env = clj.env;
      goto EVAL_TOP;
    default:
      longjmp(top, e_evalerr);
    }
  }
  else { // lambda or macro
    CLJ clj = apply(fbody, args, env, 0);
    e = clj.expr;
    env = clj.env;
    goto EVAL_TOP;
  }
  longjmp(top, e_evalerr);
}
  
 
CLJ apply(cell_ptr fbody, cell_ptr args, cell_ptr env, int do_not_evaluate_args) 
{
  CLJ clj;
    
  // the 1st elements of fbody must be a lambda or macro
  cell_ptr ftype = CAR(fbody); 
  
  if (ftype == lambda_sym) {
    // fbody is a form of (lambda (fformals...) fexpr)
    cell_ptr fformals = CAR(CDR(fbody));
    cell_ptr fexpr = CAR(CDR(CDR(fbody))); 
 
    cell_ptr actual_args, eval_list_result = NIL;
    if(do_not_evaluate_args) actual_args = args; // lisp _apply already evaluated args
    else {
      eval_list_result = eval_list(args, env);
      actual_args = eval_list_result;
    }

    clj.expr = fexpr;
    clj.env = destructive_append( pair_list(fformals, actual_args), env );

    /* eval_list is not needed, so we want to reuse */
    /* while(! ENDP(eval_list_result)) { */
    /*   MRK(CAR(eval_list_result)) = 0b0; */
    /*   eval_list_result = CDR(eval_list_result); */
    /* } */
    
    return clj;
  }
 
  else if (ftype == macro_sym) {
    // fbody is a form of (macro (mformals...) mexpr)
    cell_ptr mformals = CAR(CDR(fbody));
    cell_ptr mexpr = CAR(CDR(CDR(fbody)));
 
    // 一回目の評価は、引数評価せず環境に積んで、式を評価 = 展開
    cell_ptr macro_env = destructive_append(pair_list(mformals, args), env);
    cell_ptr expanded = eval_sex(mexpr, macro_env);
    
    if(!IS_NIL(CDR(assoc(debug_sym, env)))) {
      printf("*** macro expansion ***\n");
      print_sex(expanded);
      printf("\n");
      longjmp(top, e_macroex);  // 最初のマクロ展開でストップ
    }
 
    // 二回目の評価は、一回目の評価結果をさらに評価
    clj.expr = expanded;
    clj.env = env;
    return clj;
  }
 
  else { // not lambda, macro, or native fun
    longjmp(top, e_apply);
  }
}
