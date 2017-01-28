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
#include <string.h>
#include <stdbool.h>
#include "value.h"
#include "context.h"

static void setup_specials(lisplay_cxt_t cxt);
static void setup_functions(lisplay_cxt_t cxt);

static void define_special(lisplay_cxt_t cxt, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t proc);
static void define_builtin(lisplay_cxt_t cxt, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t proc);

static lisplay_val_t special_if(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t special_quote(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t special_lambda(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t special_define(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);

static lisplay_val_t builtin_eq(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_equal(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_null(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_atom(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_pair(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_list(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_int(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_float(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_number(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_is_symbol(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_add(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);
static lisplay_val_t builtin_sub(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);

static lisplay_cstr_t ordinal_suffix(int n);
static bool eq_values(lisplay_cxt_t cxt, lisplay_type_t type, lisplay_val_t left, lisplay_val_t right);

void lisplay_setup_builtins(lisplay_cxt_t cxt) {
  setup_specials(cxt);
  setup_functions(cxt);
}

static void setup_specials(lisplay_cxt_t cxt) {
  define_special(cxt, "if", 2, 1, special_if);
  define_special(cxt, "quote", 1, 0, special_quote);
  define_special(cxt, "lambda", 2, 0, special_lambda);
  define_special(cxt, "define", 2, 0, special_define);
}

static void setup_functions(lisplay_cxt_t cxt) {
  define_builtin(cxt, "eq?", 2, 0, builtin_eq);
  define_builtin(cxt, "equal?", 2, 0, builtin_equal);
  define_builtin(cxt, "null", 1, 0, builtin_null);
  define_builtin(cxt, "atom?", 1, 0, builtin_is_atom);
  define_builtin(cxt, "pair?", 1, 0, builtin_is_pair);
  define_builtin(cxt, "list?", 1, 0, builtin_is_list);
  define_builtin(cxt, "int?", 1, 0, builtin_is_int);
  define_builtin(cxt, "float?", 1, 0, builtin_is_float);
  define_builtin(cxt, "number?", 1, 0, builtin_is_number);
  define_builtin(cxt, "symbol?", 1, 0, builtin_is_symbol);
  define_builtin(cxt, "+", 0, -1, builtin_add);
  define_builtin(cxt, "-", 1, -1, builtin_sub);
}

static void define_special(lisplay_cxt_t cxt, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t proc) {
  lisplay_val_t special = lisplay_make_special(cxt, name, required, optional, proc);
  lisplay_define(cxt, name, special);
}

static void define_builtin(lisplay_cxt_t cxt, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t proc) {
  lisplay_val_t cfunc = lisplay_make_cfunc(cxt, name, required, optional, proc);
  lisplay_define(cxt, name, cfunc);
}

lisplay_val_t special_if(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  lisplay_val_t cond = lisplay_eval(cxt, argv[0]);

  if (lisplay_is_undef(cxt, cond)) {
    return cond;
  } else if (lisplay_is_truthy(cxt, cond)) {
    return lisplay_eval(cxt, argv[1]);
  } else {
    if (argc == 3) {
      return lisplay_eval(cxt, argv[2]);
    } else {
      return lisplay_make_nil(cxt);
    }
  }
}

static lisplay_val_t special_quote(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return argv[0];
}

static lisplay_val_t special_lambda(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  lisplay_val_t param_list = argv[0];
  if (!lisplay_is_list(cxt, param_list)) {
    lisplay_set_error(cxt, "lambda: parameter list should be a list of symbol");
    return lisplay_make_undef(cxt);
  }

  int paramc = lisplay_list_len(cxt, argv[0]);
  lisplay_cstr_t params[paramc];

  for (int i = 0; i < paramc; i++) {
    lisplay_val_t name = lisplay_cons_car(cxt, param_list);

    if (!lisplay_is_sym(cxt, name)) {
      lisplay_set_error(cxt, "lambda: parameter list should be a list of symbol");
      return lisplay_make_undef(cxt);
    }

    params[i] = lisplay_sym_cstr(cxt, name);
    param_list = lisplay_cons_cdr(cxt, param_list);
  }

  return lisplay_make_lfunc(cxt, paramc, params, cxt->stack->env, argv[1]);
}

static lisplay_val_t special_define(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  lisplay_val_t name = argv[0];
  if (!lisplay_is_sym(cxt, name)) {
    lisplay_set_error(cxt, "define: 1st argument shoud be symbol, but got %s", lisplay_typename(cxt, name));
    return lisplay_make_undef(cxt);
  }

  lisplay_val_t val = lisplay_eval(cxt, argv[1]);

  if (lisplay_has_error(cxt)) {
    return lisplay_make_undef(cxt);
  }

  lisplay_env_add(cxt, cxt->stack->prev->env, lisplay_sym_cstr(cxt, name), val);
  return name;
}

static lisplay_val_t builtin_eq(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  lisplay_val_t left = argv[0];
  lisplay_type_t left_type = lisplay_type(cxt, left);

  lisplay_val_t right = argv[1];
  lisplay_type_t right_type = lisplay_type(cxt, right);

  if (left_type != right_type) {
    return lisplay_make_false(cxt);
  }

  return lisplay_make_bool(cxt, eq_values(cxt, left_type, left, right));
}

static lisplay_val_t builtin_equal(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  lisplay_val_t left = argv[0];
  lisplay_type_t left_type = lisplay_type(cxt, left);

  lisplay_val_t right = argv[1];
  lisplay_type_t right_type = lisplay_type(cxt, right);

  if (left_type != right_type) {
    return lisplay_make_false(cxt);
  }

  if (eq_values(cxt, left_type, left, right)) {
    return lisplay_make_true(cxt);
  }

  if (left_type == LISPLAY_TYPE_INT) {
    return lisplay_make_bool(cxt, lisplay_int_val(cxt, left) == lisplay_int_val(cxt, right));
  } else if (left_type == LISPLAY_TYPE_FLOAT) {
    return lisplay_make_bool(cxt, lisplay_float_val(cxt, left) == lisplay_float_val(cxt, right));
  } else {
    return lisplay_make_false(cxt);
  }
}

static lisplay_val_t builtin_null(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return lisplay_make_bool(cxt, lisplay_is_nil(cxt, argv[0]));
}

static lisplay_val_t builtin_is_atom(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  switch (lisplay_type(cxt, argv[0])) {
  case LISPLAY_TYPE_FALSE:
  case LISPLAY_TYPE_TRUE:
  case LISPLAY_TYPE_NIL:
  case LISPLAY_TYPE_INT:
  case LISPLAY_TYPE_FLOAT:
  case LISPLAY_TYPE_SYM:
    return lisplay_make_true(cxt);
  default:
    return lisplay_make_false(cxt);
  }
}

static lisplay_val_t builtin_is_pair(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return lisplay_make_bool(cxt, lisplay_is_cons(cxt, argv[0]));
}

static lisplay_val_t builtin_is_list(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return lisplay_make_bool(cxt, lisplay_is_list(cxt, argv[0]));
}

static lisplay_val_t builtin_is_int(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return lisplay_make_bool(cxt, lisplay_is_int(cxt, argv[0]));
}

static lisplay_val_t builtin_is_float(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return lisplay_make_bool(cxt, lisplay_is_float(cxt, argv[0]));
}

static lisplay_val_t builtin_is_number(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  lisplay_type_t type = lisplay_type(cxt, argv[0]);
  return lisplay_make_bool(cxt, type == LISPLAY_TYPE_INT || type == LISPLAY_TYPE_FLOAT);
}

static lisplay_val_t builtin_is_symbol(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  return lisplay_make_bool(cxt, lisplay_is_sym(cxt, argv[0]));
}

static lisplay_val_t builtin_add(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  bool is_float = false;
  union {
    lisplay_cint_t as_int;
    lisplay_cfloat_t as_float;
  } sum;

  sum.as_int = 0;

  for (int i = 0; i < argc; i++) {
    lisplay_val_t val = argv[i];
    lisplay_type_t type = lisplay_type(cxt, val);
    if (type == LISPLAY_TYPE_INT) {
      if (is_float) {
        sum.as_float += lisplay_int_val(cxt, val);
      } else {
        sum.as_int += lisplay_int_val(cxt, val);
      }
    } else if (type == LISPLAY_TYPE_FLOAT) {
      if (is_float) {
        sum.as_float += lisplay_float_val(cxt, val);
      } else {
        sum.as_float = sum.as_int + lisplay_float_val(cxt, val);
        is_float = true;
      }
    } else {
      lisplay_set_error(cxt, "+: expect number, but %d%s argument is %s", i + 1, ordinal_suffix(i + 1), lisplay_typename(cxt, val));
      return lisplay_make_undef(cxt);
    }
  }

  if (is_float) {
    return lisplay_make_float(cxt, sum.as_float);
  } else {
    return lisplay_make_int(cxt, sum.as_int);
  }
}

static lisplay_val_t builtin_sub(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv) {
  bool is_float;
  union {
    lisplay_cint_t as_int;
    lisplay_cfloat_t as_float;
  } sum;
  lisplay_val_t first = argv[0];

  if (lisplay_is_int(cxt, first)) {
    is_float = false;
    sum.as_int = lisplay_int_val(cxt, first);
  } else if (lisplay_is_float(cxt, first)) {
    is_float = true;
    sum.as_float = lisplay_float_val(cxt, first);
  } else {
    lisplay_set_error(cxt, "+: expect number, but 1%s argument is %s", ordinal_suffix(1), lisplay_typename(cxt, first));
    return lisplay_make_undef(cxt);
  }

  for (int i = 1; i < argc; i++) {
    lisplay_val_t val = argv[i];
    lisplay_type_t type = lisplay_type(cxt, val);
    if (type == LISPLAY_TYPE_INT) {
      if (is_float) {
        sum.as_float -= lisplay_int_val(cxt, val);
      } else {
        sum.as_int -= lisplay_int_val(cxt, val);
      }
    } else if (type == LISPLAY_TYPE_FLOAT) {
      if (is_float) {
        sum.as_float -= lisplay_float_val(cxt, val);
      } else {
        sum.as_float = sum.as_int - lisplay_float_val(cxt, val);
        is_float = true;
      }
    } else {
      lisplay_set_error(cxt, "+: expect number, but %d%s argument is %s", i + 1, ordinal_suffix(i + 1), lisplay_typename(cxt, val));
      return lisplay_make_undef(cxt);
    }
  }

  if (is_float) {
    return lisplay_make_float(cxt, sum.as_float);
  } else {
    return lisplay_make_int(cxt, sum.as_int);
  }
}

static lisplay_cstr_t ordinal_suffix(int n) {
  static lisplay_cstr_t ordinal_suffix_st = "st";
  static lisplay_cstr_t ordinal_suffix_nd = "nd";
  static lisplay_cstr_t ordinal_suffix_rd = "rd";
  static lisplay_cstr_t ordinal_suffix_th = "th";

  switch (n) {
  case 11:
  case 12:
  case 13:
    return ordinal_suffix_th;
  default:
    switch (n % 10) {
    case 1:
      return ordinal_suffix_st;
    case 2:
      return ordinal_suffix_nd;
    case 3:
      return ordinal_suffix_rd;
    default:
      return ordinal_suffix_th;
    }
  }
}

static bool eq_values(lisplay_cxt_t cxt, lisplay_type_t type, lisplay_val_t left, lisplay_val_t right) {
  switch (type) {
  case LISPLAY_TYPE_FALSE:
  case LISPLAY_TYPE_TRUE:
  case LISPLAY_TYPE_NIL:
    return true;
  case LISPLAY_TYPE_SYM:
    return  strcmp(lisplay_sym_cstr(cxt, left), lisplay_sym_cstr(cxt, right)) == 0;
  default:
    return lisplay_eq_id(cxt, left, right);
  }
}
