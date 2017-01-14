#ifndef LISPLAY_VALUE_H
#define LISPLAY_VALUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct lisplay_val_t *lisplay_val_t;
typedef struct lisplay_obj_t *lisplay_obj_t;

typedef struct lisplay_obj_header_t *lisplay_obj_header_t;
struct lisplay_cxt_t;
typedef struct lisplay_cxt_t *lisplay_cxt_t;
struct lisplay_root_chunk_t;
typedef struct lisplay_root_chunk_t *lisplay_root_chunk_t;

typedef int32_t lisplay_cint_t;
typedef double lisplay_cfloat_t;
typedef const char *lisplay_cstr_t;
typedef lisplay_val_t (*lisplay_cfunc_t)(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);

/*
 * factory functions
 */

lisplay_val_t lisplay_make_root_false(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
#define lisplay_make_false(cxt) (lisplay_make_root_false((cxt), NULL))
lisplay_val_t lisplay_make_root_true(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
#define lisplay_make_true(cxt) (lisplay_make_root_true((cxt), NULL))
lisplay_val_t lisplay_make_root_nil(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
#define lisplay_make_nil(cxt) (lisplay_make_root_nil((cxt), NULL))
lisplay_val_t lisplay_make_root_int(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cint_t i);
#define lisplay_make_int(cxt, i) (lisplay_make_root_int((cxt), NULL, (i)))
lisplay_val_t lisplay_make_root_float(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cfloat_t f);
#define lisplay_make_float(cxt, f) (lisplay_make_root_float((cxt), NULL, (f)))
lisplay_val_t lisplay_make_root_sym(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t s);
#define lisplay_make_sym(cxt, s) (lisplay_make_root_sym((cxt), NULL, (s)))
lisplay_val_t lisplay_make_root_special(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t name, int required, int optional, lisplay_cfunc_t func);
#define lisplay_make_special(cxt, name, required, optional, func) (lisplay_make_root_special((cxt), NULL, (name), (required), (optional), (func)))
lisplay_val_t lisplay_make_root_cfunc(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t name, int required, int optional, lisplay_cfunc_t func);
#define lisplay_make_cfunc(cxt, name, required, optional, func) (lisplay_make_cfunc((cxt), NULL, (name), (required), (optional), (func)))
lisplay_val_t lisplay_make_root_lfunc(lisplay_cxt_t cxt, lisplay_root_chunk_t root, int paramc, lisplay_cstr_t *params, lisplay_val_t env, lisplay_val_t body);
#define lisplay_make_lfunc(cxt, paramc, params, env, body) (lisplay_make_lfunc((cxt), NULL, (paramc), (params), (env), (body)))
lisplay_val_t lisplay_make_root_cons(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_val_t car, lisplay_val_t cdr);
#define lisplay_make_cons(cxt, car, cdr) (lisplay_make_root_cons((cxt), NULL, (car), (cdr)))
lisplay_val_t lisplay_make_root_env(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_val_t prev);
#define lisplay_make_env(cxt, prev) (lisplay_make_root_env((cxt), NULL, (prev)))
lisplay_val_t lisplay_make_root_undef(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
#define lisplay_make_undef(cxt) (lisplay_make_root_undef((cxt), NULL))

/*
 * operations for memory management
 */
lisplay_obj_header_t lisplay_obj_header_of(lisplay_cxt_t cxt, lisplay_val_t val);
lisplay_obj_t lisplay_obj_of(lisplay_cxt_t cxt, lisplay_obj_header_t header);
bool lisplay_obj_header_is_val(lisplay_cxt_t cxt, lisplay_obj_header_t header);
lisplay_val_t lisplay_val_of(lisplay_cxt_t cxt, lisplay_obj_header_t header);
void lisplay_mark_children(lisplay_cxt_t cxt, lisplay_val_t val);
void lisplay_destroy_obj(lisplay_cxt_t cxt, lisplay_obj_t obj);

/*
 * operations for type
 */

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

lisplay_type_t lisplay_type(lisplay_cxt_t cxt, lisplay_val_t val);
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

/*
 * operations for int
 */
lisplay_cint_t lisplay_int_val(lisplay_cxt_t cxt, lisplay_val_t i);

/*
 * operations for float
 */
lisplay_cfloat_t lisplay_float_val(lisplay_cxt_t cxt, lisplay_val_t f);

/*
 * operations for symbol
 */
lisplay_cstr_t lisplay_sym_cstr(lisplay_cxt_t cxt, lisplay_val_t s);
lisplay_cint_t lisplay_sym_len(lisplay_cxt_t cxt, lisplay_val_t s);

/*
 * operations for special form
 */
lisplay_cstr_t lisplay_special_name(lisplay_cxt_t cxt, lisplay_val_t s);
int lisplay_special_required(lisplay_cxt_t cxt, lisplay_val_t s);
int lisplay_special_optional(lisplay_cxt_t cxt, lisplay_val_t s);
lisplay_cfunc_t lisplay_special_func(lisplay_cxt_t cxt, lisplay_val_t s);

/*
 * operations for c function
 */
lisplay_cstr_t lisplay_cfunc_name(lisplay_cxt_t cxt, lisplay_val_t f);
int lisplay_cfunc_required(lisplay_cxt_t cxt, lisplay_val_t f);
int lisplay_cfunc_optional(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_cfunc_t lisplay_cfunc_func(lisplay_cxt_t cxt, lisplay_val_t f);

/*
 * operations for lisp function
 */
int lisplay_lfunc_paramc(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_cstr_t *lisplay_lfunc_params(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_val_t lisplay_lfunc_env(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_val_t lisplay_lfunc_body(lisplay_cxt_t cxt, lisplay_val_t f);

lisplay_val_t lisplay_cons_car(lisplay_cxt_t cxt, lisplay_val_t c);
lisplay_val_t lisplay_cons_cdr(lisplay_cxt_t cxt, lisplay_val_t c);

/*
 * operations for environment
 */
lisplay_val_t lisplay_env_prev(lisplay_cxt_t cxt, lisplay_val_t e);
lisplay_val_t lisplay_env_set_prev(lisplay_cxt_t cxt, lisplay_val_t e, lisplay_val_t prev);
lisplay_val_t lisplay_env_add(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name, lisplay_val_t val);
lisplay_val_t lisplay_env_lookup(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name);
#define lisplay_env_is_defined(cxt, env, name) (lisplay_is_undef(cxt, lisplay_env_lookup(cxt, (env), (name))))
int lisplay_env_set(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name, lisplay_val_t val);

/*
 * utility
 */
int lisplay_list_len(lisplay_cxt_t cxt, lisplay_val_t val);
#define lisplay_is_list(cxt, val) (lisplay_list_len((cxt), (val)) >= 0)

#endif
