#ifndef LISPLAY_VALUE_INTERFACE_H
#define LISPLAY_VALUE_INTERFACE_H

/*
 * factory functions
 */
lisplay_val_t lisplay_make_root_false(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
lisplay_val_t lisplay_make_root_true(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
lisplay_val_t lisplay_make_root_nil(lisplay_cxt_t cxt, lisplay_root_chunk_t root);
lisplay_val_t lisplay_make_root_int(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cint_t i);
lisplay_val_t lisplay_make_root_float(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cfloat_t f);
lisplay_val_t lisplay_make_root_sym(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t s);
lisplay_val_t lisplay_make_root_special(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t func);
lisplay_val_t lisplay_make_root_cfunc(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t func);
lisplay_val_t lisplay_make_root_lfunc(lisplay_cxt_t cxt, lisplay_root_chunk_t root, int paramc, lisplay_cstr_t *params, lisplay_val_t env, lisplay_val_t body);
lisplay_val_t lisplay_make_root_cons(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_val_t car, lisplay_val_t cdr);
lisplay_val_t lisplay_make_root_env(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_val_t prev);
lisplay_val_t lisplay_make_root_undef(lisplay_cxt_t cxt, lisplay_root_chunk_t root);

lisplay_type_t lisplay_type(lisplay_cxt_t cxt, lisplay_val_t val);

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
lisplay_cproc_t lisplay_special_func(lisplay_cxt_t cxt, lisplay_val_t s);

/*
 * operations for c function
 */
lisplay_cstr_t lisplay_cfunc_name(lisplay_cxt_t cxt, lisplay_val_t f);
int lisplay_cfunc_required(lisplay_cxt_t cxt, lisplay_val_t f);
int lisplay_cfunc_optional(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_cproc_t lisplay_cfunc_func(lisplay_cxt_t cxt, lisplay_val_t f);

/*
 * operations for lisp function
 */
int lisplay_lfunc_paramc(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_cstr_t *lisplay_lfunc_params(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_val_t lisplay_lfunc_env(lisplay_cxt_t cxt, lisplay_val_t f);
lisplay_val_t lisplay_lfunc_body(lisplay_cxt_t cxt, lisplay_val_t f);

/*
 * operations for cons cell
 */
lisplay_val_t lisplay_cons_car(lisplay_cxt_t cxt, lisplay_val_t c);
lisplay_val_t lisplay_cons_cdr(lisplay_cxt_t cxt, lisplay_val_t c);

/*
 * operations for environment
 */
lisplay_val_t lisplay_env_prev(lisplay_cxt_t cxt, lisplay_val_t e);
lisplay_val_t lisplay_env_set_prev(lisplay_cxt_t cxt, lisplay_val_t e, lisplay_val_t prev);
lisplay_val_t lisplay_env_add(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name, lisplay_val_t val);
lisplay_val_t lisplay_env_lookup(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name);
int lisplay_env_set(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name, lisplay_val_t val);

/*
 * operations for memory management
 */
lisplay_obj_header_t lisplay_obj_header_of(lisplay_cxt_t cxt, lisplay_val_t val);
void lisplay_mark_children(lisplay_cxt_t cxt, lisplay_val_t val);
void lisplay_destroy_obj(lisplay_cxt_t cxt, lisplay_obj_t obj);

#endif
