#ifndef __CDB_UTILS_VECTOR_H__

#define __CDB_UTILS_VECTOR_H__

#include "utils/array.h"

#define EVEC_AC 1 // argument check error
#define EVEC_NM 2 // no memory error
#define EVEC_OB 3 // out of boundary error
#define EVEC_CP 4 // copy error

typedef struct {
    u32 length;
    u32 size;
    u32 isize;
    void *vector;
    bool (*fcopy)(void *, void *);
} Vector;

/*
    create a new vector:
        ISIZE is the element size(can also called the type of element),
        FCOPY is the function for customizing element copy
*/
errinfo new_vector(Vector *vector, u32 isize, bool (*fcopy)(void *, void *));

/*
    create a new vector from LENGTH:
        VECTOR's length == LENGTH
*/
errinfo new_vector_from_length(Vector *vector, u32 isize, u32 length,
                               bool (*fcopy)(void *, void *));

/*
    create a new vector from an existed array:
        VECTOR's size == ARRAY's size,
        VECTOR's length = ARRAY's size + 16,
        VECTOR's isize == ARRAY's isize
*/
errinfo new_vector_from_array(Vector *vector, Array *array, bool (*fcopy)(void *, void *));

/*
    create a new vector from an existed array, but reference it directly without copy:
        VECTOR's size == ARRAY's size,
        VECTOR's length = ARRAY's size,
        VECTOR's isize == ARRAY's isize
*/
errinfo new_vector_from_ref_array(Vector *vector, Array *array, bool (*fcopy)(void *, void *));

errinfo vector_push(Vector *vector, void *item);

errinfo vector_pop(Vector *vector, void *item);

errinfo vector_push_array(Vector *vector, Array *array);

errinfo vector_pop_array(Vector *vector, Array *array);

errinfo free_vector(Vector *vector);

#endif