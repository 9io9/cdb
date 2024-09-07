//
// Created by mario on 2024/9/7.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>

#include "buffer.def.h"
#include "buffer.err.h"

extern CDBBufferStatusCode pgbuffer_new(FILE* f, CDBDiskPageBuffer** pg_buf);

extern CDBBufferStatusCode pgbuffer_fill(FILE* f, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_flush(FILE* f, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_write(void* src, size_t src_size, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_read(void* dst, size_t dst_size, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_free(CDBDiskPageBuffer** pg_buf);

// no func arg check version

extern CDBBufferStatusCode pgbuffer_new_nocheck(FILE* f, CDBDiskPageBuffer** pg_buf);

extern CDBBufferStatusCode pgbuffer_fill_nocheck(FILE* f, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_flush_nocheck(FILE* f, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_write_nocheck(void* src, size_t src_size, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_read_nocheck(void* dst, size_t dst_size, CDBDiskPageBuffer* pg_buf);

extern CDBBufferStatusCode pgbuffer_free_nocheck(CDBDiskPageBuffer** pg_buf);

#endif //BUFFER_H
