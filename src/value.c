#include "value.h"

int lisplay_list_len(lisplay_cxt_t cxt, lisplay_val_t val) {
  switch (lisplay_type(cxt, val)) {
  case LISPLAY_TYPE_NIL:
    return 0;
    break;
  case LISPLAY_TYPE_CONS: {
    int tail_len = lisplay_list_len(cxt, lisplay_cons_cdr(cxt, val));
    return tail_len >= 0 ? tail_len + 1 : tail_len;
  }
  default:
    return -1;
    break;
  }
}
