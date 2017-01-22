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
#include "context.h"

typedef struct printed_list_t {
  lisplay_val_t val;
  struct printed_list_t *prev;
} *printed_list_t;

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

void fprint_val_protected(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val, printed_list_t printed);
static void fprint_cons(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val, printed_list_t printed);
static void fprint_cons_tail(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val, printed_list_t printed);
static bool is_printed_already(lisplay_cxt_t cxt, lisplay_val_t val, printed_list_t printed);

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

void lisplay_fprint_val(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val) {
  fprint_val_protected(cxt, fp, val, NULL);
}

void fprint_val_protected(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val, printed_list_t printed) {
  lisplay_type_t type = lisplay_type(cxt, val);

  switch (type) {
  case LISPLAY_TYPE_FALSE:
    fprintf(fp, "#f");
    break;
  case LISPLAY_TYPE_TRUE:
    fprintf(fp, "#t");
    break;
  case LISPLAY_TYPE_NIL:
    fprintf(fp, "()");
    break;
  case LISPLAY_TYPE_INT:
    fprintf(fp, "%d", lisplay_int_val(cxt, val));
    break;
  case LISPLAY_TYPE_FLOAT:
    fprintf(fp, "%g", lisplay_float_val(cxt, val));
    break;
  case LISPLAY_TYPE_SYM:
    fprintf(fp, "%s", lisplay_sym_cstr(cxt, val));
    break;
  case LISPLAY_TYPE_SPECIAL:
    fprintf(fp, "#<special %s>", lisplay_special_name(cxt, val));
    break;
  case LISPLAY_TYPE_CFUNC:
    fprintf(fp, "#<lambda: %s>", lisplay_cfunc_name(cxt, val));
    break;
  case LISPLAY_TYPE_LFUNC:
    fprintf(fp, "#<lambda>");
    break;
  case LISPLAY_TYPE_CONS:
    fprint_cons(cxt, fp, val, &((struct printed_list_t){ val, printed }));
    break;
  case LISPLAY_TYPE_ENV:
    fprintf(fp, "#<env>");
    break;
  case LISPLAY_TYPE_UNDEF:
    fprintf(fp, "#<undef>");
    break;
  default:
    lisplay_bug(cxt, "unreconized type: %d", type);
  }
}

static void fprint_cons(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val, printed_list_t printed) {
  fprintf(fp, "(");
  lisplay_val_t car = lisplay_cons_car(cxt, val);
  if (is_printed_already(cxt, car, printed)) {
    fprintf(fp, "...");
  } else {
    lisplay_fprint_val(cxt, fp, car);
  }

  fprint_cons_tail(cxt, fp, lisplay_cons_cdr(cxt, val), printed);
}

static void fprint_cons_tail(lisplay_cxt_t cxt, FILE *fp, lisplay_val_t val, printed_list_t printed) {
  lisplay_type_t type = lisplay_type(cxt, val);
  if (type == LISPLAY_TYPE_NIL) {
    fprintf(fp, ")");
  } else if (type == LISPLAY_TYPE_CONS) {
    fprintf(fp, " ");
    fprint_val_protected(cxt, fp, lisplay_cons_car(cxt, val), printed);
    fprint_cons_tail(cxt, fp, lisplay_cons_cdr(cxt, val), printed);
  } else {
    fprintf(fp, " . ");
    fprint_val_protected(cxt, fp, val, printed);
  }
}

static bool is_printed_already(lisplay_cxt_t cxt, lisplay_val_t val, printed_list_t printed) {
  if (printed == NULL) {
    return false;
  } else {
    if (lisplay_eq_id(cxt, val, printed->val)) {
      return true;
    } else {
      return is_printed_already(cxt, val, printed->prev);
    }
  }
}
