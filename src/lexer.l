%{
#include <string.h>
#include "scanner.h"

#define set_and_return(token) \
  do { \
    lisplay_scanner_t scanner = yyextra; \
    if (yyleng > LISPLAY_MAX_TOEN_LEN) { \
      snprintf(scanner->text, LISPLAY_MAX_TOEN_LEN, "too long token '%s'", yytext); \
      return LISPLAY_TOKEN_ERR; \
    } \
    strcpy(scanner->text, yytext); \
    return token; \
  } while(0);
%}

%option reentrant
%option noinput
%option nounput

%option prefix="lisplay_yy"
%option extra-type="lisplay_scanner_t"

%%

"." {
  set_and_return(LISPLAY_TOKEN_DOT);
}

"(" {
  set_and_return(LISPLAY_TOKEN_LPAREN);
}

")" {
  set_and_return(LISPLAY_TOKEN_RPAREN);
}

"#t" {
  set_and_return(LISPLAY_TOKEN_TRUE);
}

"#f" {
  set_and_return(LISPLAY_TOKEN_FALSE);
}

[-+][0-9]+[.][0-9]* {
  set_and_return(LISPLAY_TOKEN_FLOAT);
}

[-+][0-9]+ {
  set_and_return(LISPLAY_TOKEN_INT);
}

[-/+*%!?0-9_a-zA-Z]+ {
  set_and_return(LISPLAY_TOKEN_SYM);
}

[ \t\n] {}

. {
  lisplay_scanner_t scanner = yyextra;
  snprintf(scanner->text, LISPLAY_MAX_TOEN_LEN, "unreconized token: %s", yytext);
  return LISPLAY_TOKEN_ERR;
}

%%

int lisplay_yywrap(yyscan_t scanner) {
  return 1;
}
