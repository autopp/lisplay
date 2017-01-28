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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "context.h"
#include "heap.h"
#include "builtin.h"

static void *default_alloc(void *ptr, size_t size, void *data);
static lisplay_val_t invoke_proc(lisplay_cxt_t cxt, lisplay_val_t sexpr);
static lisplay_val_t prepare_env(lisplay_cxt_t cxt);
static void commit_env(lisplay_cxt_t cxt, lisplay_val_t base_env);
static void abort_env(lisplay_cxt_t cxt);
static void finish_call(lisplay_cxt_t cxt);
static bool check_args(lisplay_cxt_t cxt, int argc, int required, int optional, lisplay_cstr_t name);

lisplay_cxt_t lisplay_init_cxt(lisplay_cxt_t cxt, bool gc_everytime) {
  cxt->alloc_func = default_alloc;
  cxt->alloc_data = NULL;
  cxt->gc_enbaled = false;
  cxt->gc_everytime = gc_everytime;
  cxt->heap.next = NULL;
  cxt->stack = NULL;
  cxt->env_in_preparation = lisplay_make_undef(cxt);
  cxt->root.next = NULL;
  cxt->has_protected = false;
  cxt->last_error = NULL;
  prepare_env(cxt);
  lisplay_val_t bottom = lisplay_make_undef(cxt);
  commit_env(cxt, bottom);
  cxt->gc_enbaled = true;

  lisplay_setup_builtins(cxt);

  return cxt;
}

void lisplay_destroy_cxt(lisplay_cxt_t cxt) {
  // destroy stack
  lisplay_stack_t sp = cxt->stack;
  while (sp != NULL) {
    lisplay_stack_t cur = sp;
    sp = sp->prev;
    lisplay_free(cxt, cur);
  }

  // destroy root objects
  lisplay_root_chunk_t root = &cxt->root;
  while (root->next != NULL) {
    lisplay_root_chunk_t chunk = root->next;
    root->next = chunk ->next;

    lisplay_obj_header_t obj = chunk->obj;
    while (obj != NULL) {
      lisplay_obj_header_t ptr = obj;
      obj = ptr->next;
      lisplay_destroy_obj(cxt, lisplay_obj_of(cxt, ptr));
      lisplay_free(cxt, ptr);
    }

    lisplay_free(cxt, chunk);
  }

  // destroy all objects
  lisplay_obj_header_t head = &cxt->heap;
  while (head->next != NULL) {
    lisplay_obj_header_t obj_header = head->next;
    head->next = obj_header->next;
    lisplay_destroy_obj(cxt, lisplay_obj_of(cxt, obj_header));
    lisplay_free(cxt, obj_header);
  }

  lisplay_clear_error(cxt);
}

lisplay_val_t lisplay_lookup(lisplay_cxt_t cxt, lisplay_cstr_t name) {
  lisplay_val_t val = lisplay_env_lookup(cxt, cxt->stack->env, name);

  if (lisplay_is_undef(cxt, val)) {
    lisplay_set_error(cxt, "%s is not defined in current scope", name);
  }

  return val;
}

void lisplay_define(lisplay_cxt_t cxt, lisplay_cstr_t name, lisplay_val_t val) {
  lisplay_env_add(cxt, cxt->stack->env, name, val);
}

void lisplay_set_error(lisplay_cxt_t cxt, lisplay_cstr_t fmt, ...) {
  va_list varg1, varg2;
  va_start(varg1, fmt);
  va_copy(varg2, varg1);

  size_t size = vsnprintf(NULL, 0, fmt, varg1) + 1;
  va_end(varg1);

  char *error = lisplay_malloc(cxt, sizeof(char) * (size));
  vsnprintf(error, size, fmt, varg2);
  va_end(varg2);

  cxt->last_error = error;
}

void lisplay_clear_error(lisplay_cxt_t cxt) {
  if (cxt->last_error != NULL) {
    lisplay_free(cxt, cxt->last_error);
    cxt->last_error = NULL;
  }
}

lisplay_val_t lisplay_eval(lisplay_cxt_t cxt, lisplay_val_t sexpr) {
  lisplay_val_t ret;
  switch (lisplay_type(cxt, sexpr)) {
  case LISPLAY_TYPE_SYM: {
    lisplay_cstr_t name = lisplay_sym_cstr(cxt, sexpr);
    ret = lisplay_lookup(cxt, name);
    break;
  }
  case LISPLAY_TYPE_CONS: {
    if (lisplay_is_list(cxt, sexpr)) {
      ret = invoke_proc(cxt, sexpr);
    } else {
      lisplay_set_error(cxt, "procedure calling should be a list");
      ret = lisplay_make_undef(cxt);
    }
    break;
  }
  case LISPLAY_TYPE_NIL:
  case LISPLAY_TYPE_TRUE:
  case LISPLAY_TYPE_FALSE:
  case LISPLAY_TYPE_INT:
  case LISPLAY_TYPE_FLOAT: {
    ret = sexpr;
    break;
  }
  default: {
    lisplay_set_error(cxt, "unexpected type %d", lisplay_type(cxt, sexpr));
    ret = lisplay_make_undef(cxt);
    break;
  }
  }

  return ret;
}

void lisplay_fatal(lisplay_cxt_t cxt, lisplay_cstr_t fmt, ...) {
  va_list varg;
  fprintf(stderr, "LISPLAY FATAL: ");
  va_start(varg, fmt);
  vfprintf(stderr, fmt, varg);
  va_end(varg);
  fprintf(stderr, "\n");
  abort();
}

void lisplay_report_bug(lisplay_cxt_t cxt, lisplay_cstr_t filename, unsigned int line, lisplay_cstr_t funcname, lisplay_cstr_t fmt, ...) {
  va_list varg;
  fprintf(stderr, "LISPLAY BUG at %s:%u in %s\n", filename, line, funcname);
  va_start(varg, fmt);
  vfprintf(stderr, fmt, varg);
  va_end(varg);
  fprintf(stderr, "\n");
  abort();
}

static void *default_alloc(void *ptr, size_t size, void *data) {
  if (ptr == NULL) {
    return malloc(size);
  } else {
    free(ptr);
    return NULL;
  }
}

#define set_arg_names(argc, arg_names)\
  do { \
    for (int i = 0; i < (argc); i++) { \
      sprintf(arg_names[i], "%d", i + 1); \
    } \
  } while(0)

static lisplay_val_t invoke_proc(lisplay_cxt_t cxt, lisplay_val_t sexpr) {
  lisplay_val_t ret;

  int argc = lisplay_list_len(cxt, sexpr) - 1;
  int argc_digit = log10(argc) + 1;

  lisplay_val_t new_env = prepare_env(cxt);
  lisplay_val_t func_expr = lisplay_cons_car(cxt, sexpr);
  lisplay_val_t func = lisplay_eval(cxt, func_expr);

  lisplay_env_add(cxt, new_env, LISPLAY_CALLEE_NAME, func);

  switch (lisplay_type(cxt, func)) {
    case LISPLAY_TYPE_SPECIAL: {
      int required = lisplay_special_required(cxt, func);
      int optional = lisplay_special_optional(cxt, func);
      lisplay_cstr_t name = lisplay_special_name(cxt, func);

      if (!check_args(cxt, argc, required, optional, name)) {
        abort_env(cxt);
        ret = lisplay_make_undef(cxt);
        break;
      }

      char arg_names[argc][argc_digit + 1];
      lisplay_val_t argv[argc];
      set_arg_names(argc, arg_names);

      lisplay_val_t ptr = lisplay_cons_cdr(cxt, sexpr);

      for (int i = 0; i < argc; i++, ptr = lisplay_cons_cdr(cxt, ptr)) {
        lisplay_val_t arg = lisplay_cons_car(cxt, ptr);
        lisplay_env_add(cxt, new_env, arg_names[i], arg);
        argv[i] = arg;
      }

      commit_env(cxt, cxt->stack->env);
      lisplay_cproc_t cfunc = lisplay_special_func(cxt, func);
      ret = cfunc(cxt, argc, argv);
      finish_call(cxt);

      break;
    }
    case LISPLAY_TYPE_CFUNC: {
      int required = lisplay_cfunc_required(cxt, func);
      int optional = lisplay_cfunc_optional(cxt, func);
      lisplay_cstr_t name = lisplay_cfunc_name(cxt, func);

      if (!check_args(cxt, argc, required, optional, name)) {
        abort_env(cxt);
        ret = lisplay_make_undef(cxt);
        break;
      }

      char arg_names[argc][argc_digit + 1];
      lisplay_val_t argv[argc];
      set_arg_names(argc, arg_names);

      lisplay_val_t ptr = lisplay_cons_cdr(cxt, sexpr);
      for (int i = 0; i < argc; i++, ptr = lisplay_cons_cdr(cxt, ptr)) {
        lisplay_val_t arg = lisplay_eval(cxt, lisplay_cons_car(cxt, ptr));
        if (lisplay_is_undef(cxt, arg)) {
          break;
        }
        lisplay_env_add(cxt, new_env, arg_names[i], arg);
        argv[i] = arg;
      }

      if (lisplay_has_error(cxt)) {
        abort_env(cxt);
        ret = lisplay_make_undef(cxt);
      } else {
        commit_env(cxt, cxt->stack->env);
        lisplay_cproc_t cfunc = lisplay_cfunc_func(cxt, func);
        ret = cfunc(cxt, argc, argv);
        finish_call(cxt);
      }

      break;
    }
    case LISPLAY_TYPE_LFUNC: {
      int paramc = lisplay_lfunc_paramc(cxt, func);
      lisplay_cstr_t *params = lisplay_lfunc_params(cxt, func);
      if (!check_args(cxt, argc, paramc, 0, "#<lambda>")) {
        abort_env(cxt);
        ret = lisplay_make_undef(cxt);
        break;
      }
      lisplay_val_t ptr = lisplay_cons_cdr(cxt, sexpr);

      for (int i = 0; i < argc; i++, ptr = lisplay_cons_cdr(cxt, ptr)) {
        lisplay_val_t arg = lisplay_eval(cxt, lisplay_cons_car(cxt, ptr));
        if (lisplay_is_undef(cxt, arg)) {
          break;
        }
        lisplay_env_add(cxt, new_env, params[i], arg);
      }

      if (lisplay_has_error(cxt)) {
        abort_env(cxt);
        ret = lisplay_make_undef(cxt);
      } else {
        commit_env(cxt, lisplay_lfunc_env(cxt, func));
        lisplay_val_t body = lisplay_lfunc_body(cxt, func);
        ret = lisplay_eval(cxt, body);
        finish_call(cxt);
      }

      break;
    }
    case LISPLAY_TYPE_UNDEF: {
      ret = func_expr;
      abort_env(cxt);
      break;
    }
    default: {
      lisplay_set_error(cxt, "cannot call for not procedure value");
      abort_env(cxt);
      ret = lisplay_make_undef(cxt);
      break;
    }
  }

  return ret;
}

static lisplay_val_t prepare_env(lisplay_cxt_t cxt) {
  lisplay_val_t new_env = lisplay_make_env(cxt, cxt->env_in_preparation);
  cxt->env_in_preparation = new_env;

  return new_env;
}

static void commit_env(lisplay_cxt_t cxt, lisplay_val_t base_env) {
  lisplay_stack_t new_frame = lisplay_malloc(cxt, sizeof(struct lisplay_stack_t));

  lisplay_val_t new_env = cxt->env_in_preparation;
  cxt->env_in_preparation = lisplay_env_prev(cxt, new_env);
  lisplay_env_set_prev(cxt, new_env, base_env);

  new_frame->env = new_env;
  new_frame->prev = cxt->stack;
  cxt->stack = new_frame;
}

static void abort_env(lisplay_cxt_t cxt) {
  cxt->env_in_preparation = lisplay_env_prev(cxt, cxt->env_in_preparation);
}

static void finish_call(lisplay_cxt_t cxt) {
  lisplay_stack_t finished_frame = cxt->stack;
  cxt->stack = finished_frame->prev;
  lisplay_free(cxt, finished_frame);
}

static bool check_args(lisplay_cxt_t cxt, int argc, int required, int optional, lisplay_cstr_t name) {
  if (optional < 0) {
    if (argc < required) {
      lisplay_set_error(cxt, "%s: require %d or more, but got %d", name, required, argc);
      return false;
    }
  } else if (optional == 0) {
    if (argc != required) {
      lisplay_set_error(cxt, "%s: require %d, but got %d", name, required, argc);
      return false;
    }
  } else {
    int max = required + optional;
    if (argc < required || argc > max) {
      lisplay_set_error(cxt, "%s: require %d to %d, but got %d", name, required, max, argc);
      return false;
    }
  }
  return true;
}
