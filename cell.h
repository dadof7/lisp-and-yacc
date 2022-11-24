typedef unsigned int cell_ptr;

#include "hash.h"

#define TAG_CNS  0b00
#define TAG_INT  0b01
#define TAG_REAL 0b10

#define INT_TYPE int
#define REAL_TYPE float
#define INT_FORMAT "%d"
#define REAL_FORMAT "%f"

typedef struct {
  unsigned mrk:1;
  unsigned tag:2;

  union {
    struct {
      cell_ptr car:16;
      cell_ptr cdr:16;
    };
    INT_TYPE value;
    REAL_TYPE rvalue;
  };
} cns_cell;

// closure like object, expr should be evaluated in env
typedef struct {
  cell_ptr expr; // yet to be evaluated
  cell_ptr env;  // in the context of this env
} CLJ;

typedef struct {
  char *fname;
  union {
    cell_ptr (*fp)();
    CLJ (*fp2)();    
  };
  unsigned tf:2;
} fun_cell;

#define CNSTAB_SZ (1024*40)
#define SYMTAB_SZ HASH_SZ
#define STRTAB_SZ HASH_SZ

#define NIL 0
#define CNSTAB_OFFSET 1 // top cell is wasted for NIL, never used
#define SYMTAB_OFFSET (CNSTAB_SZ)
#define STRTAB_OFFSET (SYMTAB_OFFSET+SYMTAB_SZ)
#define FUNTAB_OFFSET (STRTAB_OFFSET+STRTAB_SZ)

// ----------
// | nil    | <- 0 wasted cell for NIL
// | ...    | 
// | CNSTAB | only CNSTAB is gc'ed
// | ...    |
// ----------
// | ...    |
// | SYMTAB | SYMTAB and STRTAB are equal sizes,
// | ...    | since they share the same hash table
// ----------
// | ...    |
// | STRTAB |
// | ...    |
// ----------
// | ...    |
// | FUNTAB |

extern cns_cell cnstab[];
extern fun_cell funtab[];

#define _DREF_(p) cnstab[(p)]
#define CAR(p) _DREF_(p).car
#define CDR(p) _DREF_(p).cdr
#define VAL(p) _DREF_(p).value
#define RVAL(p) _DREF_(p).rvalue
#define TAG(p) _DREF_(p).tag
#define MRK(p) _DREF_(p).mrk

#define IS_NIL(p) ((p) == NIL)
#define IS_CNS_OR_NUM(p) ((p) < CNSTAB_SZ && ! IS_NIL(p))
#define IS_CNS(p) (IS_CNS_OR_NUM(p) && TAG(p) == TAG_CNS)
#define IS_INT(p) (IS_CNS_OR_NUM(p) && TAG(p) == TAG_INT)
#define IS_REAL(p) (IS_CNS_OR_NUM(p) && TAG(p) == TAG_REAL)
#define IS_NUM(p) (IS_INT(p) || IS_REAL(p))
#define IS_SYM(p) (SYMTAB_OFFSET <= (p) && (p) < SYMTAB_OFFSET+SYMTAB_SZ)
#define IS_STR(p) (STRTAB_OFFSET <= (p) && (p) < STRTAB_OFFSET+STRTAB_SZ)
#define IS_FUN(p) (FUNTAB_OFFSET <= (p))

extern cell_ptr hash(char *str);
extern cell_ptr mk_symcell(char *name); // no duplication for the same name; OK to call many times
extern cell_ptr mk_strcell(char *str); // no duplication for the same string; OK to call many times
extern cell_ptr mk_intcell(INT_TYPE v);
extern cell_ptr mk_realcell(REAL_TYPE v);
extern cell_ptr cons(cell_ptr car, cell_ptr cdr);
extern void cns_usage();
extern void mark0();
extern void gc();
extern int is_cnstab_tight();

#define INT2CELL(v) mk_intcell(v)
#define CELL2INT(p) VAL(p)

#define REAL2CELL(v) mk_realcell(v)
#define CELL2REAL(p) RVAL(p)

#define FUN_NM(p) funtab[(p)-FUNTAB_OFFSET].fname
#define FUN_FP(p) funtab[(p)-FUNTAB_OFFSET].fp
#define FUN_FP2(p) funtab[(p)-FUNTAB_OFFSET].fp2
#define FUN_TF(p) funtab[(p)-FUNTAB_OFFSET].tf

#define SYM_NM(p) hash_tab[(p)-SYMTAB_OFFSET].s
#define STR_STR(p) hash_tab[(p)-STRTAB_OFFSET].s
