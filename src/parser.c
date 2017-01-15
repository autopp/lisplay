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
#include "parser.h"
#include "scanner.h"
#include "context.h"
#include "value.h"

static lisplay_val_t parse_sexpr(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_scanner_t scanner);
static lisplay_val_t parse_cons(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_scanner_t scanner);
static lisplay_val_t parse_cons_tail(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_scanner_t scanner);

lisplay_val_t lisplay_parse_stream(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t filename, FILE *fp) {
  struct lisplay_scanner_t scanner;
  lisplay_init_scanner(cxt, &scanner, filename, fp);
  lisplay_val_t ret = parse_sexpr(cxt, root, &scanner);
  lisplay_destroy_scanner(cxt, &scanner);

  return ret;
}

static lisplay_val_t parse_sexpr(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_scanner_t scanner) {
  lisplay_token_t token = lisplay_scanner_head(cxt, scanner);
  lisplay_val_t ret;

  switch (token) {
  case LISPLAY_TOKEN_ERR:
    lisplay_set_error(cxt, "parse error: %s", scanner->text);
    ret = lisplay_make_root_undef(cxt, root);
    break;
  case LISPLAY_TOKEN_EOS:
    lisplay_set_error(cxt, "parse error: expect '(' or atom, but <EOS> given");
    ret = lisplay_make_root_undef(cxt, root);
    break;
  case LISPLAY_TOKEN_LPAREN:
    lisplay_scanner_forward(cxt, scanner);
    ret = parse_cons(cxt, root, scanner);
    break;
  case LISPLAY_TOKEN_TRUE:
    ret = lisplay_make_root_true(cxt, root);
    lisplay_scanner_forward(cxt, scanner);
    break;
  case LISPLAY_TOKEN_FALSE:
    ret = lisplay_make_root_false(cxt, root);
    lisplay_scanner_forward(cxt, scanner);
    break;
  case LISPLAY_TOKEN_INT:
    ret = lisplay_make_root_int(cxt, root, atoi(scanner->text));
    lisplay_scanner_forward(cxt, scanner);
    break;
  case LISPLAY_TOKEN_FLOAT:
    ret = lisplay_make_root_float(cxt, root, strtod(scanner->text, NULL));
    lisplay_scanner_forward(cxt, scanner);
    break;
  case LISPLAY_TOKEN_SYM:
    ret = lisplay_make_root_sym(cxt, root, scanner->text);
    lisplay_scanner_forward(cxt, scanner);
    break;
  default:
    lisplay_set_error(cxt, "parse error: expect '(' or atom, but '%s' given", scanner->text);
    ret = lisplay_make_root_undef(cxt, root);
    break;
  }

  return ret;
}

lisplay_val_t parse_cons(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_scanner_t scanner) {
  lisplay_token_t token = lisplay_scanner_head(cxt, scanner);
  if (token == LISPLAY_TOKEN_ERR) {
    lisplay_set_error(cxt, "parse error: %s", scanner->text);
    return lisplay_make_root_undef(cxt, root);
  } else if (token == LISPLAY_TOKEN_RPAREN) {
    lisplay_scanner_forward(cxt, scanner);
    return lisplay_make_root_nil(cxt, root);
  } else {
    lisplay_val_t car = parse_sexpr(cxt, root, scanner);
    if (lisplay_is_undef(cxt, car)) {
      return car;
    }

    lisplay_val_t cdr = parse_cons_tail(cxt, root, scanner);
    if (lisplay_is_undef(cxt, cdr)) {
      return cdr;
    }

    return lisplay_make_root_cons(cxt, root, car, cdr);
  }
}

lisplay_val_t parse_cons_tail(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_scanner_t scanner) {
  lisplay_token_t token = lisplay_scanner_head(cxt, scanner);

  if (token == LISPLAY_TOKEN_ERR) {
    lisplay_set_error(cxt, "parse error: %s", scanner->text);
    return lisplay_make_root_undef(cxt, root);
  } else if (token == LISPLAY_TOKEN_EOS) {
    lisplay_set_error(cxt, "parse error: expect sexpr, dot, or ')', but <EOS> given");
    return lisplay_make_root_undef(cxt, root);
  } else if (token == LISPLAY_TOKEN_RPAREN) {
    lisplay_scanner_forward(cxt, scanner);
    return lisplay_make_root_nil(cxt, root);
  } else if (token == LISPLAY_TOKEN_DOT) {
    lisplay_scanner_forward(cxt, scanner);
    lisplay_val_t cdr = parse_sexpr(cxt, root, scanner);

    if (lisplay_is_undef(cxt, cdr)) {
      return cdr;
    }

    lisplay_token_t after_cdr = lisplay_scanner_head(cxt, scanner);
    if (after_cdr != LISPLAY_TOKEN_RPAREN) {
      if (after_cdr == LISPLAY_TOKEN_EOS) {
        lisplay_set_error(cxt, "parse error: expect ')', but <EOS> given");
      } else {
        lisplay_set_error(cxt, "parse error: expect ')', but %s given", scanner->text);
      }
      return lisplay_make_root_undef(cxt, root);
    }

    lisplay_scanner_forward(cxt, scanner);
    return cdr;
  } else {
    lisplay_val_t sexpr = parse_sexpr(cxt, root, scanner);
    if (lisplay_is_undef(cxt, sexpr)) {
      return sexpr;
    }

    lisplay_val_t cdr = parse_cons_tail(cxt, root, scanner);
    if (lisplay_is_undef(cxt, cdr)) {
      return cdr;
    }

    return lisplay_make_root_cons(cxt, root, sexpr, cdr);
  }
}
