#include "cell.h"

cell_ptr mk_symcell(char *str) {
  cell_ptr p = hash(str);
  return p + SYMTAB_OFFSET;
}

