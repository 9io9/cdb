#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "utils/string.h"

#define DEFAULT_LENGTH 16

/*
    fill end with zero-terminator
*/
static inline void fill(String *string) {
    string->string[string->size] = '\0';
}

/*
    fill end with character and increase SIZE
*/
static inline void fillc(String *string, i8 c) {
    string->string[string->size++] = c;
    string->string[string->size] = '\0';
}

/*
    fill end with const string and increase SIZE with CONSTR_LENGTH
*/
static inline void fills(String *string, constr const_string) {
    strcpy(&string->string[string->size], const_string);
    string->size += strlen(const_string);
}

/*
    test SIZE with LENGTH - 1 [one element spare for zero-terminator of c style
   string]
*/
static inline bool test_size(String *string, u32 addon) {
    return string->size + addon <= string->length - 1;
}

static errinfo expand(String *string, u32 threshold) {
    u32 new_length = (threshold + string->length) << 1;
    cstr new_string = malloc(new_length);

    if (new_string == NULL) {
        return EINFO(ESTR_NM, "no spare memory");
    }

    strcpy(new_string, string->string);

    free(string->string);

    string->string = new_string;
    string->length = new_length;

    fill(string);

    return EINFO(0, NULL);
}

errinfo new_string(String *string) {
    if (string == NULL) {
        return EINFO(ESTR_AC, "string == NULL");
    }

    string->length = DEFAULT_LENGTH;
    string->size = 0;
    string->string = malloc(string->length);

    if (string->string == NULL) {
        memset(string, 0, sizeof(String));
        return EINFO(ESTR_NM, "no spare memory");
    }

    fill(string);

    return EINFO(0, NULL);
}

errinfo new_string_from_constr(String *string, constr const_string) {
    if (string == NULL || const_string == NULL) {
        return EINFO(ESTR_AC, "string == NULL or const_string == NULL");
    }

    i32 length = strlen(const_string);

    string->length = DEFAULT_LENGTH + length;
    string->size = length;
    string->string = malloc(string->length);

    if (string->string == NULL) {
        memset(string, 0, sizeof(String));
        return EINFO(ESTR_NM, "no spare memory");
    }

    strcpy(string->string, const_string);
    fill(string);

    return EINFO(0, NULL);
}

errinfo new_string_from_length(String *string, u32 length) {
    if (string == NULL || length == 0) {
        return EINFO(ESTR_AC, "string == NULL or length == 0");
    }

    string->length = length;
    string->size = 0;
    string->string = malloc(length);

    if (string->string == NULL) {
        memset(string, 0, sizeof(String));
        return EINFO(ESTR_NM, "no spare memory");
    }

    fill(string);

    return EINFO(0, NULL);
}

errinfo string_push_i8(String *string, i8 c) {
    if (string == NULL) {
        return EINFO(ESTR_AC, "string == NULL");
    }

    if (!test_size(string, 1)) {
        errinfo e = expand(string, 1);

        if (ECHECK(e)) {
            return e;
        }
    }

    fillc(string, c);

    return EINFO(0, NULL);
}

errinfo string_push_constr(String *string, constr const_string) {
    if (string == NULL || const_string == NULL) {
        return EINFO(ESTR_AC, "string == NULL or const_string == NULL");
    }

    if (!test_size(string, strlen(const_string))) {
        errinfo e = expand(string, strlen(const_string));

        if (ECHECK(e)) {
            return e;
        }
    }

    fills(string, const_string);

    return EINFO(0, NULL);
}

errinfo free_string(String *string) {
    if (string == NULL) {
        return EINFO(ESTR_AC, "string == NULL");
    }

    if (string->string == NULL) {
        return EINFO(ESTR_AC, "string->string == NULL");
    }

    free(string->string);

    memset(string, 0, sizeof(String));

    return EINFO(0, NULL);
}

// extra string functions to complete libc <string.h>

void strtrim(cstr string, u32 *begin, u32 *end) {
    if (strlen(string) == 0) {
        *begin = *end = 0;
        return;
    }

    *begin = 0;
    *end = strlen(string) - 1;

    while (string[*begin] != '\0' && (isblank(string[*begin]) || string[*begin] == '\n')) {
        *begin += 1;
    }

    if (*begin >= *end) {
        *begin = *end;
        return;
    }

    while ((isblank(string[*end]) || string[*end] == '\n') && *end > *begin) {
        *end -= 1;
    }
}