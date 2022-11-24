#include "cell.h"

cell_ptr mk_strcell(char *str) {
  cell_ptr p = hash(str);
  return p + STRTAB_OFFSET;
}
