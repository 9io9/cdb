#ifndef __CDB_UTILS_ARRAY_H__

#define __CDB_UTILS_ARRAY_H__

#include "type.h"

typedef struct {
    u32 size;
    u32 isize;
    void* array;
}Array;
// TODO: api implement
errinfo array_get(Array* array, u32 index, void* item);

errinfo array_set(Array* array, u32 index, void* item);

errinfo array_ref_get(Array* array, u32 index, void** ptr);

#endif