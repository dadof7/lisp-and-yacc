#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include "cell.h"
#include "token.h"
#include "read.h"
#include "global.h"

/*
  exp:: number
     :: symbol
     :: '(' ')'
     :: '(' exp+ '.' exp ')'
     :: '(' exp+ ')'
 */

cell_ptr parse_sex(token_t* token) 
{
  cell_ptr expr;

  if(token == (void*)0) {
    longjmp(top, e_eof);
  }

  else if(token->kind == 'N') {
    return INT2CELL((INT_TYPE) token->lval);
  }

  else if(token->kind == 'R') {
    return REAL2CELL((REAL_TYPE) token->dval);
  }
  
  else if(token->kind == '\'') {
    token = get_token();
    expr = parse_sex(token);
    return cons(quote_sym, cons(expr, NIL));
  }

  else if(token->kind == '`') {
    token = get_token();
    expr = parse_sex(token);
    return cons(bquote_sym, cons(expr, NIL));
  }

  else if(token->kind == ',') {
    token = get_token();
    expr = parse_sex(token);
    return cons(comma_sym, cons(expr, NIL));
  }

  else if(token->kind == '@') {
    token = get_token();
    expr = parse_sex(token);
    return cons(comma_at_sym, cons(expr, NIL));
  }
  
  else if(token->kind == 'S') {
    // printf("symbol\n");
    // nil is a keyword???
    if(strncmp(token->str, "nil", strlen(token->str)+1) == 0) return NIL; // nil is a keyword-like sym
    else if(strncmp(token->str, "$", strlen(token->str)+1) == 0) return cons(quote_sym, cons(the_last_expr, NIL));
    else return mk_symcell(token->str);
  }

  else if(token->kind == 'G') {
    return mk_strcell(token->str);
  }

  // expr -> ()
  //      -> ( expr expr* )
  //      -> ( expr expr* . expr )
  else if(token->kind == '(') { 
    token = get_token(); // token just after (

    // expr -> ()
    if(token->kind == ')') {
      return NIL;
    }

    // expr -> ( expr expr* )
    //      -> ( expr expr* . expr )
    //           ^
    //           expr next line is here
    expr = parse_sex(token);	// just after '('
    
    cell_ptr expr2, head, tail;
    head = tail = cons(expr, NIL);
    
    while(1) {
      token = get_token();
      
      if(token->kind == ')')
	return head;

      else if(token->kind == '.') {
	token = get_token();
	expr2 = parse_sex(token);
	token = get_token();
	if(token->kind != ')') longjmp(top, e_parseerr);
	CDR(tail) = expr2;
	return head;
      }

      else { // expr continues...
	expr2 = parse_sex(token);
	cell_ptr new_tail = cons(expr2, NIL);
	CDR(tail) = new_tail;
	tail = new_tail;
      }
    }
  }
  longjmp(top, e_parseerr);
}

cell_ptr read_sex(token_t* token) 
{
  return parse_sex(token);
}
