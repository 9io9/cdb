//
// Created by mario on 2024/9/7.
//

#ifndef BUFFER_DEF_H
#define BUFFER_DEF_H

#include "../db_apis/consts.h"

typedef struct CDBDiskPageBuffer {
    size_t _buf_index;
    char _pg_buf[DISKPG_SIZE];
}CDBDiskPageBuffer;

#endif //BUFFER_DEF_H
