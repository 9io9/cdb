#ifndef __CDB_UTILS_ARRAY_H__

#define __CDB_UTILS_ARRAY_H__

#include "type.h"

#define EARR_AC 1 // argument check error
#define EARR_OB 2 // out of boundary error
#define EARR_CP 3 // copy error

typedef struct {
    u32 size;
    u32 isize;
    void *array;
    bool (*fcopy) (void*, void*);
} Array;

errinfo array_get(Array *array, u32 index, void *item);

errinfo array_set(Array *array, u32 index, void *item);

errinfo array_ref_get(Array *array, u32 index, void **ptr);

#endif