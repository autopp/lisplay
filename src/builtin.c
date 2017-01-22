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

static void setup_specials(lisplay_cxt_t cxt);
static void setup_functions(lisplay_cxt_t cxt);

#define define_special(cxt, name, required, optional, func) (lisplay_define((cxt), (name), lisplay_make_special((cxt), (name), (required), (optional), (func))))
#define define_func(cxt, name, required, optional, func) (lisplay_define((cxt), (name), lisplay_make_cfunc((cxt), (name), (required), (optional), (func))))

static lisplay_val_t special_if(lisplay_cxt_t cxt, int argc, lisplay_val_t *argv);

void lisplay_setup_builtins(lisplay_cxt_t cxt) {
  setup_specials(cxt);
  setup_functions(cxt);
}

static void setup_specials(lisplay_cxt_t cxt) {
  define_func(cxt, "if", 2, 1, special_if);
}

static void setup_functions(lisplay_cxt_t cxt) {
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
