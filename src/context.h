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
#ifndef LISPLAY_CONTEXT_H
#define LISPLAY_CONTEXT_H

#include <stdbool.h>
#include "value.h"

typedef struct lisplay_stack_t {
  lisplay_val_t env;
  struct lisplay_stack_t *prev;
} *lisplay_stack_t;

typedef struct lisplay_root_chunk_t {
  struct lisplay_obj_header_t *obj, *last;
  struct lisplay_root_chunk_t *next;
} *lisplay_root_chunk_t;

typedef struct lisplay_cxt_t {
  bool gc_enbaled;
  struct lisplay_obj_header_t heap;
  struct lisplay_stack_t *stack;
  lisplay_val_t env_in_preparation;
  struct lisplay_root_chunk_t root;
  lisplay_val_t protected;
  bool has_protected;
  lisplay_cstr_t last_error;
} *lisplay_cxt_t;

#define LISPLAY_CALLEE_NAME "lisplay: callee"

lisplay_cxt_t lisplay_init_cxt(lisplay_cxt_t cxt);
void lisplay_destroy_cxt(lisplay_cxt_t cxt);

lisplay_val_t lisplay_lookup(lisplay_cxt_t cxt, lisplay_cstr_t name);
void lisplay_define(lisplay_cxt_t cxt, lisplay_cstr_t name, lisplay_val_t val);
void lisplay_set_error(lisplay_cxt_t cxt, lisplay_cstr_t error, ...);
void lisplay_clear_error(lisplay_cxt_t cxt);
#define lisplay_has_error(cxt) (cxt->last_error != NULL)

#define lisplay_protect(cxt, val) \
  do { \
    if (cxt->has_protected) { \
      lisplay_fatal(cxt, "double protection detected"); \
    } else { \
      cxt->has_protected = true; \
      cxt->protected = val; \
    } \
  } while (0)

#define lisplay_unprotect(cxt) \
  do { \
    if (cxt->has_protected) { \
      cxt->has_protected = false; \
    } else { \
      lisplay_fatal(cxt, "double unprotection detected"); \
    } \
  } while (0)

void lisplay_fatal(lisplay_cxt_t cxt, lisplay_cstr_t fmt, ...);
void lisplay_report_bug(lisplay_cxt_t cxt, lisplay_cstr_t filename, unsigned int line, lisplay_cstr_t funcname, lisplay_cstr_t fmt, ...);
#define lisplay_bug(cxt, ...) (lisplay_report_bug((cxt), __FILE__, __LINE__, __func__,  __VA_ARGS__))

#endif
