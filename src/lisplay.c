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
#include "context.h"
#include "parser.h"
#include "heap.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  const char *filename;
  FILE *fp;
  filename = argv[1];

  if (filename == NULL) {
    filename = "<stdin>";
    fp = stdin;
  } else {
    fp = fopen(filename, "r");
  }

  struct lisplay_cxt_t context;
  lisplay_cxt_t cxt = &context;

  lisplay_init_cxt(cxt);

  lisplay_root_chunk_t root = lisplay_create_root(cxt);
  lisplay_val_t sexpr = lisplay_parse_stream(cxt, root, filename, fp);

  if (lisplay_has_error(cxt)) {
    fprintf(stderr, "parse error: %s\n", cxt->last_error);
    exit(1);
  }
  lisplay_val_t val = lisplay_eval(cxt, sexpr);
  lisplay_fprint_val(cxt, stdout, val);
  printf("\n");
  lisplay_destroy_cxt(cxt);
  if (fp != stdin) {
    fclose(fp);
  }

  return 0;
}
