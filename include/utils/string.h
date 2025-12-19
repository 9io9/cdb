#ifndef __CDB_UTILS_STR_H__
#define __CDB_UTILS_STR_H__

#include "type.h"

#define ESTR_AC 1 // argument check error
#define ESTR_NM 2 // no memory error

typedef struct {
    u32 length;
    u32 size;
    cstr string;
} String;

/*
    create a new string:
        length = 16, size = 0, string = [15, '\0']
*/
errinfo new_string(String *string);

/*
    create a new string from const string:
        length = CONSTR_LENGTH + 16, size = CONSTR_LENGTH, string = [CONSTR, '\0']
*/
errinfo new_string_from_constr(String *string, constr const_string);

/*
    create a new string from length:
        length = LENGTH, size = 0, string = [LENGTH, '\0']
*/
errinfo new_string_from_length(String *string, u32 length);

/*
    push a character to string:
        if size + 1 > length: string will expand, new length will be (old length + 1) * 2
*/
errinfo string_push_i8(String *string, i8 c);

/*
    push a const string to string:
        if size + CONSTR_LENGTH > length: string will expand, new length will be (old length +
   CONSTR_LENGTH) * 2
*/
errinfo string_push_constr(String *string, constr const_string);

/*
    free an existed string
*/
errinfo free_string(String *string);

// extra string functions to complete libc <string.h>

/*
    trim a cstr, return non-blank string with BEGIN and END
*/
void strtrim(cstr string, u32 *begin, u32 *end);

#endif