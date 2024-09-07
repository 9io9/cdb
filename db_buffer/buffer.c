//
// Created by mario on 2024/9/7.
//

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "../db_apis/name.h"

#include "buffer.def.h"
#include "buffer.err.h"

CDBBufferStatusCode pgbuffer_new(FILE* f, CDBDiskPageBuffer** pg_buf) {
    if (f == NULL || pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    *pg_buf = malloc(sizeof(CDBDiskPageBuffer));

#ifdef __MEM_SENSITIVE__
    if (*pg_buf == NULL) {
        return __BUF_ERR__(NoMem);
    }
#endif

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_new_nocheck(FILE* f, CDBDiskPageBuffer** pg_buf) {
    *pg_buf = malloc(sizeof(CDBDiskPageBuffer));

#ifdef __MEM_SENSITIVE__
    if (*pg_buf == NULL) {
        return __BUF_ERR__(NoMem);
    }
#endif

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_fill(FILE* f, CDBDiskPageBuffer* pg_buf) {
    if (f == NULL || pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    if (fread(pg_buf->_pg_buf, sizeof(char), DISKPG_SIZE, f) == 0) {
        return __BUF_ERR__(FileRead);
    }

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_fill_nocheck(FILE* f, CDBDiskPageBuffer* pg_buf) {
    if (fread(pg_buf->_pg_buf, sizeof(char), DISKPG_SIZE, f) == 0) {
        return __BUF_ERR__(FileRead);
    }

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_flush(FILE* f, CDBDiskPageBuffer* pg_buf) {
    if (f == NULL || pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    if (fwrite(pg_buf->_pg_buf, sizeof(char), pg_buf->_buf_index, f) == 0) {
        return __BUF_ERR__(FileWrite);
    }

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_flush_nocheck(FILE* f, CDBDiskPageBuffer* pg_buf) {
    if (fwrite(pg_buf->_pg_buf, sizeof(char), pg_buf->_buf_index, f) == 0) {
        return __BUF_ERR__(FileWrite);
    }

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_write(void* src, size_t src_size, CDBDiskPageBuffer* pg_buf) {
    if (src == NULL || src_size == 0 || pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    if (src_size > DISKPG_SIZE) {
        return __BUF_ERR__(Overflow);
    }

    pg_buf->_buf_index = src_size;

    memcpy(pg_buf->_pg_buf, src, src_size);

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_write_nocheck(void* src, size_t src_size, CDBDiskPageBuffer* pg_buf) {
    if (src_size > DISKPG_SIZE) {
        return __BUF_ERR__(Overflow);
    }

    pg_buf->_buf_index = src_size;

    memcpy(pg_buf->_pg_buf, src, src_size);

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_read(void* dst, size_t dst_size, CDBDiskPageBuffer* pg_buf) {
    if (dst == NULL || dst_size == 0 || pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    if (dst_size < pg_buf->_buf_index) {
        return __BUF_ERR__(Overflow);
    }

    memcpy(dst, pg_buf->_pg_buf, dst_size);

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_read_nocheck(void* dst, size_t dst_size, CDBDiskPageBuffer* pg_buf) {
    if (dst_size < pg_buf->_buf_index) {
        return __BUF_ERR__(Overflow);
    }

    memcpy(dst, pg_buf->_pg_buf, dst_size);

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_free(CDBDiskPageBuffer** pg_buf) {
    if (pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    if (*pg_buf == NULL) {
        return __BUF_ERR__(FuncArg);
    }

    free(*pg_buf);

    *pg_buf = NULL;

    return __BUF_SUCCESS__;
}

CDBBufferStatusCode pgbuffer_free_nocheck(CDBDiskPageBuffer** pg_buf) {
    free(*pg_buf);

    *pg_buf = NULL;

    return __BUF_SUCCESS__;
}