#include <stdio.h>
#include <setjmp.h>

#include "cell.h"
#include "global.h"
#include "print.h"

void _print_sex(cell_ptr e, FILE* f) 
{
  if (IS_NIL(e)) {
    fprintf(f, "nil");
  } 

  else if (IS_SYM(e)) {
    fprintf(f, "%s", SYM_NM(e));
  }

  else if (IS_STR(e)) {
    fprintf(f, "\"%s\"", STR_STR(e));
  }
  
  else if (IS_INT(e)) {
    fprintf(f, INT_FORMAT, CELL2INT(e));
  }

  else if (IS_REAL(e)) {
    fprintf(f, REAL_FORMAT, CELL2REAL(e));
  }

  else if(IS_FUN(e)) {
    fprintf(f, "#%s", FUN_NM(e));
  }

  else if (IS_CNS(e)) {
    char *q = 0;
    cell_ptr care = CAR(e);
    if(care == quote_sym) q = "'";
    else if(care == bquote_sym) q = "`";
    else if(care == comma_sym) q = ",";
    else if(care == comma_at_sym) q = ",@";

    if(q) { // quoted
      fprintf(f, "%s", q);
      _print_sex(CAR(CDR(e)), f);       
    }
    else { // not quote => printed w/ '(' and ')'
      fprintf(f, "(");
      while (1) {
	_print_sex(CAR(e), f);
	e = CDR(e);
	if (IS_NIL(e)) break;
	if (IS_INT(e) || IS_REAL(e) || IS_SYM(e) || IS_STR(e) || IS_FUN(e)) {
	  fprintf(f, " . ");
	  _print_sex(e, f);
	  break;
	}
	fprintf(f, " ");
      } // while(1)
    fprintf(f, ")");
    } // not quote
  } // IS_CNS
  
  else {
    fprintf(stderr, "*** Printer failed ***\n");
  }
}

void print_sex(cell_ptr e) 
{
  _print_sex(e, stdout);
}
