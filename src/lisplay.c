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
#include <unistd.h>
#include "context.h"
#include "parser.h"
#include "heap.h"

struct lisplay_options_t {
  bool print_mode;
  bool print_error_mode;
  bool gc_everytime;
  const char *filename;
  FILE *fp;
};

static void parse_options(int argc, char **argv, struct lisplay_options_t *options);

int main(int argc, char **argv) {
  struct lisplay_options_t options;
  parse_options(argc, argv, &options);

  struct lisplay_cxt_t context;
  lisplay_cxt_t cxt = &context;

  lisplay_init_cxt(cxt, options.gc_everytime);

  lisplay_root_chunk_t root = lisplay_create_root(cxt);
  lisplay_val_t sexprs = lisplay_parse_sexprs(cxt, root, options.filename, options.fp);

  if (lisplay_has_error(cxt)) {
    fprintf(stderr, "parse error: %s\n", cxt->last_error);
    exit(1);
  } else {
    while (!lisplay_is_nil(cxt, sexprs)) {
      lisplay_val_t sexpr = lisplay_cons_car(cxt, sexprs);
      lisplay_val_t val = lisplay_eval(cxt, sexpr);

      if (lisplay_has_error(cxt)) {
        if (options.print_error_mode) {
          printf("runtime error: %s\n", cxt->last_error);
          lisplay_clear_error(cxt);
        } else {
          fprintf(stderr, "runtime error: %s\n", cxt->last_error);
          lisplay_clear_error(cxt);
          exit(1);
        }
      } else {
        if (options.print_mode) {
          lisplay_fprint_val(cxt, stdout, val);
          printf("\n");
        }
      }
      sexprs = lisplay_cons_cdr(cxt, sexprs);
    }
  }

  lisplay_destroy_cxt(cxt);
  if (options.fp != stdin) {
    fclose(options.fp);
  }

  return 0;
}

static void parse_options(int argc, char **argv, struct lisplay_options_t *options) {
  options->print_mode = false;
  options->print_error_mode = false;
  options->gc_everytime = false;
  int opt;
  opterr = 0;
  while ((opt = getopt(argc, argv, "peG")) != -1) {
    switch (opt) {
    case 'p':
      options->print_mode = true;
      break;
    case 'e':
      options->print_error_mode = true;
      break;
    case 'G':
      options->gc_everytime = true;
      break;
    case '?':
      fprintf(stderr, "unknown option -%c\n", optopt);
      exit(1);
      break;
    }
  }

  if (optind >= argc) {
    options->filename = "<stdin>";
    options->fp = stdin;
  } else {
    options->filename = argv[optind];
    options->fp = fopen(options->filename, "r");
    if (options->fp == NULL) {
      fprintf(stderr, "cannot open %s\n", options->filename);
      exit(1);
    }
  }
}
