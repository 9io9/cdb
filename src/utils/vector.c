#include <malloc.h>
#include <string.h>

#include "utils/vector.h"

#define DEFAULT_LENGTH 16

static inline void *accat(void *base, u32 offset, u32 size) {
    return base + offset * size;
}

static errinfo expand(Vector *vector, u32 threshold) {
    u32 new_length = (vector->length + threshold) << 1;

    void *new_vector = malloc(new_length * vector->isize);

    if (new_vector == NULL) {
        return EINFO(EVEC_NM, "no spare memory for vector");
    }

    memcpy(new_vector, vector->vector, vector->size * vector->isize);

    free(vector->vector);

    vector->vector = new_vector;
    vector->length = new_length;

    return EINFO(0, NULL);
}

errinfo new_vector(Vector *vector, u32 isize, bool (*fcopy)(void *, void *)) {
    if (vector == NULL || isize == 0) {
        return EINFO(EVEC_AC, "vector == NULL or isize == 0");
    }

    vector->isize = isize;
    vector->size = 0;
    vector->length = DEFAULT_LENGTH;
    vector->vector = malloc(vector->isize * vector->length);
    vector->fcopy = fcopy;

    if (vector->vector == NULL) {
        return EINFO(EVEC_NM, "no spare memory for vector");
    }

    return EINFO(0, NULL);
}

errinfo new_vector_from_length(Vector *vector, u32 isize, u32 length,
                               bool (*fcopy)(void *, void *)) {
    if (vector == NULL || isize == 0 || length == 0) {
        return EINFO(EVEC_AC, "vector == NULL or isize == 0 or length == 0");
    }

    vector->isize = isize;
    vector->length = length;
    vector->size = 0;
    vector->vector = malloc(vector->length * vector->isize);
    vector->fcopy = fcopy;

    if (vector->vector == NULL) {
        return EINFO(EVEC_NM, "no spare memory for vector");
    }

    return EINFO(0, NULL);
}

errinfo new_vector_from_array(Vector *vector, Array *array, bool (*fcopy)(void *, void *)) {
    if (vector == NULL || array == NULL) {
        return EINFO(EVEC_AC, "vector == NULL or array == NULL");
    }

    if (array->array == NULL || array->isize == 0) {
        return EINFO(EVEC_AC, "array invalid (array == NULL, size or isize == 0)");
    }

    vector->isize = array->isize;
    vector->size = array->size;
    vector->length = array->size + DEFAULT_LENGTH;
    vector->vector = malloc(vector->length * vector->isize);

    if (vector->vector == NULL) {
        return EINFO(EVEC_NM, "no spare memory for vector");
    }

    if (fcopy == NULL) {
        memcpy(vector->vector, array->array, array->size * array->isize);
    } else {
        for (u32 i = 0; i < array->size; ++i) {
            bool copy_result = fcopy(accat(vector->vector, i, vector->isize),
                                     accat(array->array, i, array->isize));

            if (!copy_result) {
                free(vector->vector);
                memset(vector, 0, sizeof(Vector));
                return EINFO(EVEC_CP, "copy function returns false");
            }
        }
    }

    return EINFO(0, NULL);
}

errinfo new_vector_from_ref_array(Vector *vector, Array *array, bool (*fcopy)(void *, void *)) {
    if (vector == NULL || array == NULL) {
        return EINFO(EVEC_AC, "vector == NULL or array == NULL");
    }

    if (array->array == NULL || array->size == 0 || array->isize == 0) {
        return EINFO(EVEC_AC, "array invalid (array == NULL, size or isize == 0)");
    }

    vector->isize = array->isize;
    vector->size = array->size;
    vector->length = array->size;
    vector->vector = array->array;
    vector->fcopy = fcopy;

    return EINFO(0, NULL);
}

errinfo vector_push(Vector *vector, void *item) {
    if (vector == NULL || item == NULL) {
        return EINFO(EVEC_AC, "vector == NULL or item == NULL");
    }

    if (vector->size == vector->length) {
        errinfo e = expand(vector, 1);

        if (ECHECK(e)) {
            return e;
        }
    }

    if (vector->fcopy == NULL) {
        memcpy(accat(vector->vector, vector->size, vector->isize), item, vector->isize);
    } else {
        bool copy_result = vector->fcopy(
            accat(vector->vector, vector->size, vector->isize),
            item
        );

        if (!copy_result) {
            return EINFO(EVEC_CP, "copy function returns false");
        }
    }

    vector->size += 1;

    return EINFO(0, NULL);
}

errinfo vector_pop(Vector *vector, void *item) {
    if (vector == NULL || item == NULL) {
        return EINFO(EVEC_AC, "vector == NULL or item == NULL");
    }

    if (vector->size == 0) {
        return EINFO(EVEC_OB, "no more element for pop");
    }

    if (vector->fcopy == NULL) {
        memcpy(item, accat(vector->vector, vector->size - 1, vector->isize), vector->isize);
    } else {
        bool copy_result = vector->fcopy(item, accat(vector->vector, vector->size - 1, vector->isize));
    
        if (!copy_result) {
            return EINFO(EVEC_CP, "copy function returns false");
        }
    }

    vector->size -= 1;

    return EINFO(0, NULL);
}

errinfo vector_push_array(Vector *vector, Array *array) {
    if (vector == NULL || array == NULL) {
        return EINFO(EVEC_AC, "vector == NULL or array == NULL");
    }

    if (vector->size + array->size > vector->length) {
        errinfo e = expand(vector, array->size);

        if (ECHECK(e)) {
            return e;
        }
    }

    if (vector->fcopy == NULL) {
        memcpy(
            accat(vector->vector, vector->size, vector->isize),
            accat(array->array, 0, array->isize),
            array->isize * array->size    
        );
    } else {
        for (u32 i = 0; i < array->size; ++i) {
            bool copy_result = vector->fcopy(
                accat(vector->vector, vector->size + i, vector->isize),
                accat(array->array, i, array->isize)
            );

            if (!copy_result) {
                return EINFO(EVEC_CP, "copy function returns false");
            }
        }
    }

    vector->size += array->size;

    return EINFO(0, NULL);
}

errinfo vector_pop_array(Vector *vector, Array *array) {
    if (vector == NULL || array == NULL) {
        return EINFO(EVEC_AC, "vector == NULL or array == NULL");
    }

    if (vector->size < array->size) {
        return EINFO(EVEC_OB, "no more elements for pop");
    }

    if (vector->fcopy == NULL) {
        memcpy(
            accat(array->array, 0, array->isize),
            accat(vector->vector, vector->size - array->size, vector->isize),
            array->size * array->isize
        );
    } else {
        for (u32 i = 0; i < array->size; ++i) {
            bool copy_result = vector->fcopy(
                accat(array->array, i, array->isize),
                accat(vector->vector, vector->size - i - 1, vector->isize)
            );

            if (!copy_result) {
                return EINFO(EVEC_CP, "copy function returns false");
            }
        }
    }

    vector->size -= array->size;

    return EINFO(0, NULL);
}

errinfo free_vector(Vector *vector) {
    if (vector == NULL) {
        return EINFO(EVEC_AC, "vector == NULL");
    }

    if (vector->vector == NULL) {
        memset(vector, 0, sizeof(Vector));
        return EINFO(EVEC_AC, "vector->vector == NULL");
    }

    free(vector->vector);
    memset(vector, 0, sizeof(Vector));

    return EINFO(0, NULL);
}