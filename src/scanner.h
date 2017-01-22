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
#ifndef LISPLAY_SCANNER_H
#define LISPLAY_SCANNER_H

#include <stdio.h>
#include <stdbool.h>

#include "context.h"

struct lisplay_scanner_t;
typedef struct lisplay_scanner_t *lisplay_scanner_t;

typedef enum {
  LISPLAY_TOKEN_ERR = -1,
  LISPLAY_TOKEN_EOS,
  LISPLAY_TOKEN_LPAREN,
  LISPLAY_TOKEN_RPAREN,
  LISPLAY_TOKEN_DOT,
  LISPLAY_TOKEN_QUOTE,
  LISPLAY_TOKEN_TRUE,
  LISPLAY_TOKEN_FALSE,
  LISPLAY_TOKEN_INT,
  LISPLAY_TOKEN_FLOAT,
  LISPLAY_TOKEN_SYM
} lisplay_token_t;

#define LISPLAY_MAX_TOEN_LEN 255

typedef void *yyscan_t;

struct lisplay_scanner_t {
  lisplay_cxt_t cxt;
  yyscan_t state;
  lisplay_cstr_t filename;
  FILE *fp;
  bool is_buffered;
  lisplay_token_t buffered_token;
  char text[LISPLAY_MAX_TOEN_LEN + 1];
};

lisplay_scanner_t lisplay_init_scanner(lisplay_cxt_t cxt, lisplay_scanner_t scanner, lisplay_cstr_t filename, FILE *fp);
void lisplay_destroy_scanner(lisplay_cxt_t cxt, lisplay_scanner_t scanner);

lisplay_token_t lisplay_scanner_head(lisplay_cxt_t cxt, lisplay_scanner_t scanner);
void lisplay_scanner_forward(lisplay_cxt_t cxt, lisplay_scanner_t scanner);

#endif
