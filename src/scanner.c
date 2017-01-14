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
