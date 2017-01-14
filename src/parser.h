#ifndef LISPLAY_PARSER_H
#define LISPLAY_PARSER_H

#include <stdio.h>
#include "value.h"
#include "context.h"

lisplay_val_t lisplay_parse_stream(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t filename, FILE *fp);

#endif
