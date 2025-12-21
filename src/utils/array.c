#include <string.h>

#include "utils/array.h"

static inline void* accat(void* base, u32 offset, u32 size) {
    return base + offset * size;
}

errinfo array_get(Array *array, u32 index, void *item) {
    if (array == NULL || item == NULL) {
        return EINFO(EARR_AC, "array == NULL or item == NULL");
    }
    
    if (index >= array->size) {
        return EINFO(EARR_OB, "out of boundary of array");
    }

    if (array->fcopy == NULL) {
        memcpy(
            item,
            accat(array->array, index, array->isize),
            array->isize
        );
    } else {
        bool copy_result = array->fcopy(
            item,
            accat(array->array, index, array->isize)
        );

        if (!copy_result) {
            return EINFO(EARR_CP, "copy function returns false");
        }
    }

    return EINFO(0, NULL);
}

errinfo array_set(Array *array, u32 index, void *item) {
    if (array == NULL || item == NULL) {
        return EINFO(EARR_AC, "array == NULL or item == NULL");
    }

    if (index >= array->size) {
        return EINFO(EARR_OB, "out of boundary of array");
    }

    if (array->fcopy == NULL) {
        memcpy(
            accat(array->array, index, array->isize),
            item,
            array->isize
        );
    } else {
        bool copy_result = array->fcopy(
            accat(array->array, index, array->isize),
            item
        );

        if (!copy_result) {
            return EINFO(EARR_CP, "copy function returns false");
        }
    }

    return EINFO(0, NULL);
}

errinfo array_ref_get(Array *array, u32 index, void **ptr) {
    if (array == NULL || ptr == NULL) {
        return EINFO(EARR_AC, "array == NULL or ptr == NULL");
    }

    if (index >= array->size) {
        return EINFO(EARR_OB, "out of boundary of array");
    }

    *ptr = accat(array->array, index, array->isize);

    return EINFO(0, NULL);
}