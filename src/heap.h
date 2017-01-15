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
#ifndef LISPLAY_HEAP_H
#define LISPLAY_HEAP_H

#define lisplay_as_obj_header(p) ((lisplay_obj_header_t)(p))

void lisplay_mark_val(lisplay_cxt_t cxt, lisplay_val_t val);

/**
 * Same as standard malloc expect that it may run GC
 * @param  cxt
 * @param  size
 * @return      address of allocated memory
 */
void *lisplay_malloc(lisplay_cxt_t cxt, size_t size);
#define lisplay_malloc_for(cxt, type) lisplay_malloc((cxt), sizeof(type))

lisplay_cstr_t lisplay_strdup(lisplay_cxt_t cxt, lisplay_cstr_t str);
#define lisplay_free(cxt, p) ((cxt)->alloc_func((void *)(p), 0, (cxt)->alloc_data))

lisplay_root_chunk_t lisplay_create_root(lisplay_cxt_t cxt);
void lisplay_release_root(lisplay_cxt_t cxt, lisplay_root_chunk_t chunk);

lisplay_obj_header_t lisplay_alloc_root_obj(lisplay_cxt_t cxt, lisplay_root_chunk_t chunk, size_t size);
#define lisplay_alloc_root_obj_for(cxt, chunk, type) (lisplay_alloc_root_obj((cxt), (chunk), sizeof(type)))

#define lisplay_alloc_obj(cxt, size) (lisplay_alloc_root_obj((cxt), NULL, (size)))
#define lisplay_alloc_obj_for(cxt, type) (lisplay_alloc_obj((cxt), sizeof(type)))

void lisplay_collect_garbage(lisplay_cxt_t cxt);

#endif
