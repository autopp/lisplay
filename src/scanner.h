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
