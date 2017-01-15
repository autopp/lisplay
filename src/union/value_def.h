#ifndef LISPLAY_VALUE_DEF_H
#define LISPLAY_VALUE_DEF_H

union lisplay_val_t {
  struct { struct lisplay_obj_header_t header; } dummy;
};

typedef union lisplay_val_t *lisplay_val_t;

struct lisplay_obj_t;
typedef struct lisplay_obj_t *lisplay_obj_t;

#endif
