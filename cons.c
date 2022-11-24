#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include "cell.h"
#include "global.h"

/* ************
 * *** 重要 ***
 * ************
 * 大域変数はプログラム初期化時に0クリアされる.
 * これはcnstab[0]=NILの要素が全て0になることを意味する.
 * となると、NIL.car=NIL.cdr=0つまり、NILのcarもcdrもNILになることが保証される.
 */
cns_cell cnstab[CNSTAB_SZ];

static int cns_scanned_this_far = CNSTAB_OFFSET;
static int live_cns;
static int live_num;

static void sweep(cell_ptr p);

cell_ptr cons(cell_ptr car, cell_ptr cdr) 
{
  cell_ptr p;
  
  for(; cns_scanned_this_far < CNSTAB_SZ; cns_scanned_this_far++) {
    p = cns_scanned_this_far;
    if(MRK(p) == 0b0) {
      MRK(p) = 0b1;
      CAR(p) = car;
      CDR(p) = cdr;
      TAG(p) = TAG_CNS;
      return p;
    }
  }
  longjmp(top, e_consof);
}

void mark0()
{
  int i;
  cell_ptr p;

  for(i=CNSTAB_OFFSET; i < CNSTAB_SZ; i++) { 
    p = i;
    MRK(p) = 0b0;
  }
  
  // also do not forget; they may be lost
  the_last_expr = NIL;
  the_last_func = NIL;
  the_last_args = NIL;
}

static void sweep(cell_ptr p)
{
  if(IS_CNS_OR_NUM(p)) {
    if(MRK(p) == 0b1) return; // visited already

    MRK(p) = 0b1;

    if(IS_CNS(p)) {
      live_cns++;
      sweep(CAR(p));
      sweep(CDR(p));
    }
    else live_num++; // must be an int or real cell
  }
}

int gc_flag = 0;

void gc()
{
  cns_scanned_this_far = CNSTAB_OFFSET;
  live_cns = live_num = 0;

  mark0();
  sweep(top_level_env);

  gc_flag = 0;
  fprintf(stderr, "*gc*\n");
  fprintf(stderr, "live cns cells: %d\n", live_cns);
  fprintf(stderr, "live num cells: %d\n", live_num);
  hash_usage();
}

cell_ptr mk_intcell(INT_TYPE v) {
  cell_ptr baby = cons(NIL, NIL);
  VAL(baby) = v;
  TAG(baby) = TAG_INT;
  return baby;
}


cell_ptr mk_realcell(REAL_TYPE rv) {
  cell_ptr baby = cons(NIL, NIL);
  RVAL(baby) = rv;
  TAG(baby) = TAG_REAL;
  return baby;
}

void cns_usage()
{
  fprintf(stderr, "cns pointer scanned by this far: %d/%d %.2f%%\n", cns_scanned_this_far, CNSTAB_SZ, cns_scanned_this_far * 100.0 / CNSTAB_SZ);
}

#define THRESHOLD 60
int is_cnstab_tight() 
{
  return 100 * cns_scanned_this_far / CNSTAB_SZ > THRESHOLD; // where to start gc
}
