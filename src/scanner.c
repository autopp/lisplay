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
#include "scanner.h"
#include "lexer.h"

lisplay_scanner_t lisplay_init_scanner(lisplay_cxt_t cxt, lisplay_scanner_t scanner, lisplay_cstr_t filename, FILE *fp) {
  scanner->cxt = cxt;
  lisplay_yylex_init(&(scanner->state));
  lisplay_yyset_in(fp, scanner->state);
  lisplay_yyset_extra(scanner, scanner->state);
  scanner->filename = filename;
  scanner->fp = fp;
  scanner->is_buffered = false;

  return scanner;
}

void lisplay_destroy_scanner(lisplay_cxt_t cxt, lisplay_scanner_t scanner) {
  lisplay_yylex_destroy(scanner->state);
}

lisplay_token_t lisplay_scanner_head(lisplay_cxt_t cxt, lisplay_scanner_t scanner) {
  if (!scanner->is_buffered) {
    scanner->buffered_token = lisplay_yylex(scanner->state);
  }
  return scanner->buffered_token;
}

void lisplay_scanner_forward(lisplay_cxt_t cxt, lisplay_scanner_t scanner) {
  if (scanner->is_buffered) {
    scanner->is_buffered = false;
  } else {
    lisplay_yylex(scanner->state);
  }
}
