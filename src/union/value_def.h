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
#ifndef LISPLAY_VALUE_DEF_H
#define LISPLAY_VALUE_DEF_H

typedef struct lisplay_false_t {
  struct lisplay_obj_header_t header;
} *lisplay_false_t;

typedef struct lisplay_true_t {
  struct lisplay_obj_header_t header;
} *lisplay_true_t;

typedef struct lisplay_nil_t {
  struct lisplay_obj_header_t header;
} *lisplay_nil_t;

typedef struct lisplay_int_t {
  struct lisplay_obj_header_t header;
  lisplay_cint_t val;
} *lisplay_int_t;

typedef struct lisplay_float_t {
  struct lisplay_obj_header_t header;
  lisplay_cfloat_t val;
} *lisplay_float_t;

typedef struct lisplay_sym_t {
  struct lisplay_obj_header_t header;
  lisplay_cstr_t val;
} *lisplay_sym_t;

typedef struct lisplay_special_t {
  struct lisplay_obj_header_t header;
  lisplay_cstr_t name;
  int required, optional;
  lisplay_cproc_t func;
} *lisplay_special_t;

typedef struct lisplay_cfunc_t {
  struct lisplay_obj_header_t header;
  lisplay_cstr_t name;
  int required, optional;
  lisplay_cproc_t func;
} *lisplay_cfunc_t;

typedef struct lisplay_lfunc_t {
  struct lisplay_obj_header_t header;
  int paramc;
  lisplay_cstr_t *params;
  lisplay_val_t env;
  lisplay_val_t body;
} *lisplay_lfunc_t;

typedef struct lisplay_cons_t {
  struct lisplay_obj_header_t header;
  lisplay_val_t car;
  lisplay_val_t cdr;
} *lisplay_cons_t;

typedef struct lisplay_env_entry_t {
  lisplay_cstr_t name;
  lisplay_val_t val;
  struct lisplay_env_entry_t *next;
} *lisplay_env_entry_t;

typedef struct lisplay_env_t {
  struct lisplay_obj_header_t header;
  lisplay_env_entry_t entries;
  lisplay_val_t prev;
} *lisplay_env_t;

typedef struct lisplay_undef_t {
  struct lisplay_obj_header_t header;
} *lisplay_undef_t;

union lisplay_val_t {
  struct { struct lisplay_obj_header_t header; } as_any;
  struct lisplay_false_t as_false;
  struct lisplay_true_t as_true;
  struct lisplay_nil_t as_nil;
  struct lisplay_int_t as_int;
  struct lisplay_float_t as_float;
  struct lisplay_sym_t as_sym;
  struct lisplay_special_t as_special;
  struct lisplay_cfunc_t as_cfunc;
  struct lisplay_lfunc_t as_lfunc;
  struct lisplay_cons_t as_cons;
  struct lisplay_env_t as_env;
  struct lisplay_undef_t as_undef;
};

struct lisplay_obj_t;

#endif
