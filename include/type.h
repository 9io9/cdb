#ifndef __CDB_TYPE_H__

#define __CDB_TYPE_H__

#include <stdbool.h>

#define CAST(x, type) ((type)(x))

#if __SIZEOF_POINTER__ == 8

#    define MODE64 1

typedef long long i64;
typedef unsigned long long u64;
#endif

typedef int i32;
typedef short i16;
typedef char i8;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef const char *constr;
typedef char *cstr;

typedef struct {
    i32 ecode;
    u32 line;
    constr function;
    constr file;
    constr emsg;
} errinfo;

#define EINFO(ec, msg)                                                                             \
    (errinfo) {                                                                                    \
        .ecode = ec, .line = __LINE__, .function = __FUNCTION__, .file = __FILE__, .emsg = msg     \
    }

#define ECHECK(e) ((e).ecode != 0 && (e).emsg != NULL)
#define EPRINT(e)                                                                                  \
    printf("error(%d) occurs: %s in line %u of function %s in file %s\n", (e).ecode, (e).emsg,     \
           (e).line, (e).function, (e).file)
#endif