#ifndef __CDB_UTILS_VECTOR_H__

#define __CDB_UTILS_VECTOR_H__

#include "utils/array.h"

typedef struct {
    u32 length;
    u32 size;
    u32 isize;
    void* vector;
}Vector;
// TODO: api implement
errinfo new_vector(Vector* vector, u32 isize);

errinfo new_vector_from_length(Vector* vector, u32 isize, u32 length);

errinfo new_vector_from_array(Vector* vector, Array* array);

errinfo vector_push(Vector* vector, void* item);

errinfo vector_pop(Vector* vector, void* item);

errinfo vector_push_array(Vector* vector, Array* array);

errinfo vector_pop_array(Vector* vector, Array* array);

errinfo free_vector(Vector* vector);

#endif