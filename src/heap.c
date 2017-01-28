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
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void mark_objects(lisplay_cxt_t cxt);
static void sweep_heap(lisplay_cxt_t cxt);

#define mark_obj(obj) do (obj)->mark = true; while (0)
#define obj_is_marked(obj) ((obj)->mark)
#define unmark_obj(obj)  do (obj)->mark = false; while (0)

void lisplay_mark_val(lisplay_cxt_t cxt, lisplay_val_t val) {
  lisplay_obj_header_t obj = lisplay_obj_header_of(cxt, val);

  if (obj == NULL || obj_is_marked(obj)) {
    return;
  }

  mark_obj(obj);

  lisplay_mark_children(cxt, val);
}

void *lisplay_malloc(lisplay_cxt_t cxt, size_t size) {
  void *p = cxt->alloc_func(NULL, size, cxt->alloc_data);
  if (cxt->gc_enbaled && cxt->gc_everytime ) lisplay_collect_garbage(cxt);
  if (p == NULL) {
    if (cxt->gc_enbaled) {
      lisplay_collect_garbage(cxt);
      p = cxt->alloc_func(NULL, size, cxt->alloc_data);
      lisplay_fatal(cxt, "memory allocation is failed");
    } else {
      lisplay_fatal(cxt, "memory allocation is failed (GC is disabled)");
    }
  }

  return p;
}

const char *lisplay_strdup(lisplay_cxt_t cxt, lisplay_cstr_t str) {
  char *new_str = lisplay_malloc(cxt, sizeof(char) * (strlen(str) + 1));
  strcpy(new_str, str);
  return new_str;
}

lisplay_root_chunk_t lisplay_create_root(lisplay_cxt_t cxt) {
  lisplay_root_chunk_t chunk = lisplay_malloc(cxt, sizeof(struct lisplay_root_chunk_t));

  chunk->obj = chunk->last = NULL;
  chunk->next = cxt->root.next;
  cxt->root.next = chunk;

  return chunk;
}

void lisplay_release_root(lisplay_cxt_t cxt, lisplay_root_chunk_t chunk) {
  lisplay_root_chunk_t ptr = &cxt->root;

  while (ptr->next != NULL && ptr->next != chunk) {
    ptr = ptr->next;
  }

  if (ptr->next == NULL) {
    lisplay_bug(cxt, "root chunk %p is not found", chunk);
  }

  // move root objs to root
  if (chunk->obj != NULL) {
    chunk->last->next = cxt->heap.next;
    cxt->heap.next = chunk->obj;
  }

  // detach and free chunk
  ptr->next = chunk->next;
  lisplay_free(cxt, chunk);
}

lisplay_obj_header_t lisplay_alloc_root_obj(lisplay_cxt_t cxt, lisplay_root_chunk_t chunk, size_t size) {
  lisplay_obj_header_t obj = lisplay_malloc(cxt, size);

  obj->mark = false;

  if (chunk == NULL) {
    // setup for normal heap object
    obj->next = cxt->heap.next;
    cxt->heap.next = obj;
  } else {
    // setup for root object
    obj->next = chunk->obj;
    chunk->obj = obj;
    if (chunk->last == NULL) {
      chunk->last = obj;
    }
  }

  return obj;
}

void lisplay_collect_garbage(lisplay_cxt_t cxt) {
  // Mark phase
  mark_objects(cxt);

  // Sweep phase
  sweep_heap(cxt);
}

static void mark_objects(lisplay_cxt_t cxt) {
  for (lisplay_stack_t sp = cxt->stack; sp != NULL; sp = sp->prev) {
    lisplay_mark_val(cxt, sp->env);
  }

  for (lisplay_root_chunk_t chunk = cxt->root.next; chunk != NULL; chunk = chunk->next) {
  }

  lisplay_mark_val(cxt, cxt->env_in_preparation);

  if (cxt->has_protected) {
    lisplay_mark_val(cxt, cxt->protected);
  }
}

static void sweep_heap(lisplay_cxt_t cxt) {
  lisplay_obj_header_t p = &cxt->heap;
  while (p->next != NULL) {
    lisplay_obj_header_t obj_header = p->next;
    if (obj_is_marked(obj_header)) {
      unmark_obj(obj_header);
      p = obj_header;
    } else {
      p->next = obj_header->next;
      lisplay_destroy_obj(cxt, lisplay_obj_of(cxt, obj_header));
      lisplay_free(cxt, obj_header);
    }
  }
}
