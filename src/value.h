#ifndef LISPLAY_VALUE_H
#define LISPLAY_VALUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct lisplay_obj_header_t {
  struct lisplay_obj_header_t *next;
  bool mark;
} *lisplay_obj_header_t;

struct lisplay_cxt_t;
typedef struct lisplay_cxt_t *lisplay_cxt_t;

struct lisplay_root_chunk_t;
typedef struct lisplay_root_chunk_t *lisplay_root_chunk_t;

typedef enum {
  LISPLAY_TYPE_FALSE,
  LISPLAY_TYPE_TRUE,
  LISPLAY_TYPE_NIL,
  LISPLAY_TYPE_INT,
  LISPLAY_TYPE_FLOAT,
  LISPLAY_TYPE_SYM,
  LISPLAY_TYPE_SPECIAL,
  LISPLAY_TYPE_CFUNC,
  LISPLAY_TYPE_LFUNC,
  LISPLAY_TYPE_CONS,
  LISPLAY_TYPE_ENV,
  LISPLAY_TYPE_UNDEF
} lisplay_type_t;

#include "value_decl.h"

typedef int32_t lisplay_cint_t;
typedef double lisplay_cfloat_t;
typedef const char *lisplay_cstr_t;
typedef lisplay_val_t (*lisplay_cproc_t)(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);

#include "value_def.h"

#include "value_interface.h"

#define lisplay_obj_of(cxt, header) ((lisplay_obj_t)(header))

/*
 * create value as non root
 */

#define lisplay_make_false(cxt) (lisplay_make_root_false((cxt), NULL))
#define lisplay_make_true(cxt) (lisplay_make_root_true((cxt), NULL))
#define lisplay_make_nil(cxt) (lisplay_make_root_nil((cxt), NULL))
#define lisplay_make_int(cxt, i) (lisplay_make_root_int((cxt), NULL, (i)))
#define lisplay_make_float(cxt, f) (lisplay_make_root_float((cxt), NULL, (f)))
#define lisplay_make_sym(cxt, s) (lisplay_make_root_sym((cxt), NULL, (s)))
#define lisplay_make_special(cxt, name, required, optional, func) (lisplay_make_root_special((cxt), NULL, (name), (required), (optional), (func)))
#define lisplay_make_cfunc(cxt, name, required, optional, func) (lisplay_make_cfunc((cxt), NULL, (name), (required), (optional), (func)))
#define lisplay_make_lfunc(cxt, paramc, params, env, body) (lisplay_make_lfunc((cxt), NULL, (paramc), (params), (env), (body)))
#define lisplay_make_cons(cxt, car, cdr) (lisplay_make_root_cons((cxt), NULL, (car), (cdr)))
#define lisplay_make_env(cxt, prev) (lisplay_make_root_env((cxt), NULL, (prev)))
#define lisplay_make_undef(cxt) (lisplay_make_root_undef((cxt), NULL))

/*
 * operations for type
 */

#define lisplay_is_false(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_FALSE)
#define lisplay_is_true(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_TRUE)
#define lisplay_is_nil(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_NIL)
#define lisplay_is_int(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_INT)
#define lisplay_is_float(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_FLOAT)
#define lisplay_is_sym(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_SYM)
#define lisplay_is_special(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_SPECIAL)
#define lisplay_is_cfunc(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_CFUNC)
#define lisplay_is_lfunc(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_LFUNC)
#define lisplay_is_cons(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_CONS)
#define lisplay_is_env(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_ENV)
#define lisplay_is_undef(cxt, val) (lisplay_type((cxt), (val)) == LISPLAY_TYPE_UNDEF)

#define lisplay_is_truthy(cxt, val) (!(lisplay_is_false((cxt), (val))))

#define lisplay_env_is_defined(cxt, env, name) (lisplay_is_undef(cxt, lisplay_env_lookup(cxt, (env), (name))))

int lisplay_list_len(lisplay_cxt_t cxt, lisplay_val_t val);
#define lisplay_is_list(cxt, val) (lisplay_list_len((cxt), (val)) >= 0)

#endif
