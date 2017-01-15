#ifndef VALUE_DECL_H
#define VALUE_DECL_H

union lisplay_val_t;
typedef union lisplay_val_t *lisplay_val_t;
typedef lisplay_val_t lisplay_obj_t;

#define LISPLAY_OBJ_HEADER_EXTRA \
  lisplay_type_t type;

#endif
