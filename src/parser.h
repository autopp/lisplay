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
#ifndef LISPLAY_PARSER_H
#define LISPLAY_PARSER_H

#include <stdio.h>
#include "value.h"
#include "context.h"

lisplay_val_t lisplay_parse_sexprs(lisplay_cxt_t cxt, lisplay_root_chunk_t root, lisplay_cstr_t filename, FILE *fp);

#endif
