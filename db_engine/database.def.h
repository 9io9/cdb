//
// Created by mario on 2024/9/7.
//

#ifndef DATABASE_DEF_H
#define DATABASE_DEF_H

#include <stddef.h>
#include "engine.def.h"

typedef struct CDBDataBaseHeader {
    size_t _db_id;
    size_t _db_tab_num;
    time_t _db_crt_timestamp;
    size_t _db_end_offset;
}CDBDataBaseHeader;

#endif //DATABASE_DEF_H
