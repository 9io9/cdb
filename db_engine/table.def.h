//
// Created by mario on 2024/9/1.
//

#ifndef TABLE_DEF_H
#define TABLE_DEF_H

#include "../third_party/cstr/cstr.def.h"
#include "../third_party/cvla/cvla.def.h"

typedef enum CDBTableColumnType {
    Integer = 0,
    CharArray
}CDBTableColumnType;

typedef struct CDBTableColumn {
    CDBTableColumnType _col_type;
    CStringRef* _col_name;
}CDBTableColumn;

typedef struct CDBDiskTableColumn {
    char _col_name_len[1];
    char _col_type[4];
#if __MACHINE_WORD_SIZE__ == 64
    char _col_offset[8];
#define TABCOL_NAME_MAX 50
    char _col_name[51];
#elif __MACHINE_WORD_SIZE__ == 32
    char _col_offset[4];
#define TABCOL_NAME_MAX 54
    char _col_name[55];
#endif
}CDBDiskTableColumn;

typedef struct CDBTableHeader {
    size_t _tab_id;
    size_t _cols_num;
    size_t _rows_num;
    time_t _tab_crt_timestamp;
    size_t _tab_end_offset;
    CVLArray* _tab_cols;
}CDBTableHeader;

typedef struct CDBDiskTableHeader {
    char _file_type[8];
#if __MACHINE_WORD_SIZE__ == 64
    char _tab_id[8];
    char _tab_end_offset[8];
    char _tab_cols_num[8];
    char _tab_rows_num[8];
    char _tab_crt_timestamp[8];
#elif __MACHINE_WORD_SIZE__ == 32
    char _tab_id[4];
    char _tab_end_offset[4];
    char _tab_cols_num[4];
    char _tab_rows_num[4];
    char _tab_crt_timestamp[4];
#endif
}CDBDiskTableHeader;

#endif //TABLE_DEF_H
