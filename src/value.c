/**
 * Copyright 2017 autopp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "value.h"

static lisplay_cstr_t typenames[] = {
  "false",
  "true",
  "nil",
  "int",
  "float",
  "sym",
  "special",
  "cfunc",
  "lfunc",
  "cons",
  "env",
  "undef"
};

static lisplay_cstr_t unknown_typename = "unknown";

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

lisplay_cstr_t lisplay_typename(lisplay_cxt_t cxt, lisplay_val_t val) {
  lisplay_type_t type = lisplay_type(cxt, val);
  if (type >= 0 && type < LISPLAY_NUM_OF_TYPE ) {
    return typenames[type];
  } else {
    return unknown_typename;
  }
}
