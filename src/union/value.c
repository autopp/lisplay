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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../value.h"
#include "../heap.h"
#include "../context.h"

#define as_val(x) ((lisplay_val_t)(x))

static lisplay_env_entry_t lookup_env_entry(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name);

/*
 * factory functions
 */
lisplay_val_t lisplay_make_root_false(lisplay_cxt_t cxt, lisplay_root_chunk_t root) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_false_t);
  obj->type = LISPLAY_TYPE_FALSE;
  return as_val(obj);
}

lisplay_val_t lisplay_make_root_true(lisplay_cxt_t cxt, lisplay_root_chunk_t root) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_true_t);
  obj->type = LISPLAY_TYPE_TRUE;
  return as_val(obj);
}

lisplay_val_t lisplay_make_root_nil(lisplay_cxt_t cxt, lisplay_root_chunk_t root) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_nil_t);
  obj->type = LISPLAY_TYPE_NIL;
  return as_val(obj);
}

lisplay_val_t lisplay_make_root_int(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cint_t i) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_int_t);
  obj->type = LISPLAY_TYPE_INT;
  lisplay_val_t val = as_val(obj);
  val->as_int.val = i;
  return val;
}

lisplay_val_t lisplay_make_root_float(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cfloat_t f) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_float_t);
  obj->type = LISPLAY_TYPE_FLOAT;
  lisplay_val_t val = as_val(obj);
  val->as_float.val = f;
  return val;
}

lisplay_val_t lisplay_make_root_sym(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t s) {
  lisplay_cstr_t new_s = lisplay_strdup(cxt, s);
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_sym_t);
  obj->type = LISPLAY_TYPE_SYM;
  lisplay_val_t val = as_val(obj);
  val->as_sym.val = new_s;
  return val;
}

lisplay_val_t lisplay_make_root_special(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t func) {
  lisplay_cstr_t new_name = lisplay_strdup(cxt, name);
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_special_t);
  obj->type = LISPLAY_TYPE_SPECIAL;
  lisplay_val_t val = as_val(obj);
  val->as_special.name = new_name;
  val->as_special.required = required;
  val->as_special.optional = optional;
  val->as_special.func = func;
  return val;
}

lisplay_val_t lisplay_make_root_cfunc(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t name, int required, int optional, lisplay_cproc_t func) {
  lisplay_cstr_t new_name = lisplay_strdup(cxt, name);
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_cfunc_t);
  obj->type = LISPLAY_TYPE_CFUNC;
  lisplay_val_t val = as_val(obj);
  val->as_cfunc.name = new_name;
  val->as_cfunc.required = required;
  val->as_cfunc.optional = optional;
  val->as_cfunc.func = func;
  return val;
}

lisplay_val_t lisplay_make_root_lfunc(lisplay_cxt_t cxt, lisplay_root_chunk_t root, int paramc, lisplay_cstr_t *params, lisplay_val_t env, lisplay_val_t body) {
  lisplay_cstr_t *new_params = NULL;
  if (paramc != 0) {
    size_t param_lens[paramc];
    size_t param_len_sum = 0;
    for (int i = 0; i < paramc; i++) {
      int len = strlen(params[i]);
      param_lens[i] = len;
      param_len_sum += len;
    }

    new_params = lisplay_malloc(cxt, sizeof(lisplay_cstr_t) * paramc);
    char *buf = lisplay_malloc(cxt, sizeof(char) * (param_len_sum + paramc));
    for (int i = 0; i < paramc; i++) {
      strcpy(buf, params[i]);
      new_params[i] = buf;
      buf += param_lens[i] + 1;
    }
  }

  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_lfunc_t);
  obj->type = LISPLAY_TYPE_LFUNC;
  lisplay_val_t val = as_val(obj);
  val->as_lfunc.paramc = paramc;
  val->as_lfunc.params = new_params;
  val->as_lfunc.env = env;
  val->as_lfunc.body = body;

  return val;
}

lisplay_val_t lisplay_make_root_cons(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_val_t car, lisplay_val_t cdr) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_cons_t);
  lisplay_val_t val = as_val(obj);
  obj->type = LISPLAY_TYPE_CONS;
  val->as_cons.car = car;
  val->as_cons.cdr = cdr;

  return val;
}

lisplay_val_t lisplay_make_root_env(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_val_t prev) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_env_t);

  lisplay_val_t val = as_val(obj);
  obj->type = LISPLAY_TYPE_ENV;
  val->as_env.entries = NULL;
  val->as_env.prev = prev;

  return val;
}

lisplay_val_t lisplay_make_root_undef(lisplay_cxt_t cxt, lisplay_root_chunk_t root) {
  lisplay_obj_header_t obj = lisplay_alloc_root_obj_for(cxt, root, struct lisplay_undef_t);
  obj->type = LISPLAY_TYPE_UNDEF;

  return as_val(obj);
}

lisplay_type_t lisplay_type(lisplay_cxt_t cxt, lisplay_val_t val) {
  return val->as_any.header.type;
}

/*
 * operations for int
 */
lisplay_cint_t lisplay_int_val(lisplay_cxt_t cxt, lisplay_val_t i) {
  return i->as_int.val;
}

/*
 * operations for float
 */
lisplay_cfloat_t lisplay_float_val(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_float.val;
}

/*
 * operations for symbol
 */
lisplay_cstr_t lisplay_sym_cstr(lisplay_cxt_t cxt, lisplay_val_t s) {
  return s->as_sym.val;
}

lisplay_cint_t lisplay_sym_len(lisplay_cxt_t cxt, lisplay_val_t s) {
  return strlen(s->as_sym.val);
}

/*
 * operations for special form
 */
lisplay_cstr_t lisplay_special_name(lisplay_cxt_t cxt, lisplay_val_t s) {
  return s->as_special.name;
}

int lisplay_special_required(lisplay_cxt_t cxt, lisplay_val_t s) {
  return s->as_special.required;
}

int lisplay_special_optional(lisplay_cxt_t cxt, lisplay_val_t s) {
  return s->as_special.optional;
}

lisplay_cproc_t lisplay_special_func(lisplay_cxt_t cxt, lisplay_val_t s) {
  return s->as_special.func;
}

/*
 * operations for c function
 */
lisplay_cstr_t lisplay_cfunc_name(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_cfunc.name;
}

int lisplay_cfunc_required(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_cfunc.required;
}

int lisplay_cfunc_optional(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_cfunc.optional;
}

lisplay_cproc_t lisplay_cfunc_func(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_cfunc.func;
}

/*
 * operations for lisp function
 */
int lisplay_lfunc_paramc(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_lfunc.paramc;
}

lisplay_cstr_t *lisplay_lfunc_params(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_lfunc.params;
}

lisplay_val_t lisplay_lfunc_env(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_lfunc.env;
}

lisplay_val_t lisplay_lfunc_body(lisplay_cxt_t cxt, lisplay_val_t f) {
  return f->as_lfunc.body;
}

/*
 * operations for cons cell
 */
lisplay_val_t lisplay_cons_car(lisplay_cxt_t cxt, lisplay_val_t c) {
  return c->as_cons.car;
}

lisplay_val_t lisplay_cons_cdr(lisplay_cxt_t cxt, lisplay_val_t c) {
  return c->as_cons.cdr;
}

/*
 * operations for environment
 */
lisplay_val_t lisplay_env_prev(lisplay_cxt_t cxt, lisplay_val_t e) {
  return e->as_env.prev;
}

lisplay_val_t lisplay_env_set_prev(lisplay_cxt_t cxt, lisplay_val_t e, lisplay_val_t prev) {
  e->as_env.prev = prev;
  return e;
}

lisplay_val_t lisplay_env_add(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name, lisplay_val_t val) {
  lisplay_env_entry_t entry = lisplay_malloc_for(cxt, struct lisplay_env_entry_t);

  entry->name = lisplay_strdup(cxt, name);
  entry->val = val;
  entry->next = env->as_env.entries;
  env->as_env.entries = entry;

  return val;
}

lisplay_val_t lisplay_env_lookup(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name) {
  lisplay_env_entry_t entry = lookup_env_entry(cxt, env, name);
  return entry != NULL ? entry->val : lisplay_make_undef(cxt);
}

int lisplay_env_set(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name, lisplay_val_t val) {
  lisplay_env_entry_t entry = lookup_env_entry(cxt, env, name);

  if (entry != NULL) {
    entry->val = val;
    return 1;
  } else {
    return 0;
  }
}

static lisplay_env_entry_t lookup_env_entry(lisplay_cxt_t cxt, lisplay_val_t env, lisplay_cstr_t name) {
  for (lisplay_env_entry_t entry = env->as_env.entries; entry != NULL; entry = entry->next) {
    if (strcmp(entry->name, name) == 0) {
      return entry;
    }
  }

  lisplay_val_t prev = env->as_env.prev;
  return lisplay_is_undef(cxt, prev) ? NULL : lookup_env_entry(cxt, prev, name);
}

bool lisplay_eq_id(lisplay_cxt_t cxt, lisplay_val_t v1, lisplay_val_t v2) {
  return v1 == v2;
}

lisplay_obj_header_t lisplay_obj_header_of(lisplay_cxt_t cxt, lisplay_val_t val) {
  return &val->as_any.header;
}

void lisplay_mark_children(lisplay_cxt_t cxt, lisplay_val_t val) {
  switch (lisplay_type(cxt, val)) {
  case LISPLAY_TYPE_FALSE:
  case LISPLAY_TYPE_TRUE:
  case LISPLAY_TYPE_NIL:
  case LISPLAY_TYPE_INT:
  case LISPLAY_TYPE_FLOAT:
  case LISPLAY_TYPE_SYM:
  case LISPLAY_TYPE_SPECIAL:
  case LISPLAY_TYPE_CFUNC:
  case LISPLAY_TYPE_UNDEF:
    // nothing to do
    break;
  case LISPLAY_TYPE_LFUNC:
    lisplay_mark_val(cxt, val->as_lfunc.env);
    lisplay_mark_val(cxt, val->as_lfunc.body);
    break;
  case LISPLAY_TYPE_CONS:
    lisplay_mark_val(cxt, val->as_cons.car);
    lisplay_mark_val(cxt, val->as_cons.cdr);
    break;
  case LISPLAY_TYPE_ENV:
    for (lisplay_env_entry_t entry = val->as_env.entries; entry != NULL; entry = entry->next) {
      lisplay_mark_val(cxt, entry->val);
    }
    lisplay_mark_val(cxt, val->as_env.prev);
    break;
  default:
    lisplay_bug(cxt, "marking unreconized type: %d", lisplay_type(cxt, val));
    break;
  }
}

void lisplay_destroy_obj(lisplay_cxt_t cxt, lisplay_obj_t obj) {
  switch (obj->as_any.header.type) {
  case LISPLAY_TYPE_FALSE:
  case LISPLAY_TYPE_TRUE:
  case LISPLAY_TYPE_NIL:
  case LISPLAY_TYPE_INT:
  case LISPLAY_TYPE_FLOAT:
  case LISPLAY_TYPE_CONS:
  case LISPLAY_TYPE_UNDEF:
    // nothing to do
    break;
  case LISPLAY_TYPE_SYM:
    lisplay_free(cxt, obj->as_sym.val);
    break;
  case LISPLAY_TYPE_SPECIAL:
    lisplay_free(cxt, obj->as_special.name);
    break;
  case LISPLAY_TYPE_CFUNC:
    lisplay_free(cxt, obj->as_cfunc.name);
    break;
  case LISPLAY_TYPE_LFUNC:
    if (obj->as_lfunc.paramc > 0) {
      lisplay_free(cxt, obj->as_lfunc.params[0]);
      lisplay_free(cxt, obj->as_lfunc.params);
    }
    break;
  case LISPLAY_TYPE_ENV:
    for (lisplay_env_entry_t entry = obj->as_env.entries; entry != NULL; ) {
      lisplay_env_entry_t cur = entry;
      entry = entry->next;
      lisplay_free(cxt, cur->name);
      lisplay_free(cxt, cur);
    }
    break;
  default:
    lisplay_bug(cxt, "destroy unreconized type: %d", obj->as_any.header.type);
    break;
  }
}
