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
    size_t _col_offset;
    CStringRef* _col_name;
}CDBTableColumn;

typedef struct CDBDiskTableColumn {
    char _col_type[4];
#if __MACHINE_WORD_SIZE__ == 64
    char _col_offset[8];
    char _col_name[116];
#elif __MACHINE_WORD_SIZE__ == 32
    char _col_offset[4];
    char _col_name[120];
#endif
}CDBDiskTableColumn;

typedef struct CDBTableHeader {
    size_t _tab_version;
    size_t _tab_end_offset;
    CVLArray* _tab_cols;
}CDBTableHeader;

#define DISK_PGSIZE 4096

typedef struct CDBDiskTableHeader {
#if __MACHINE_WORD_SIZE__ == 64
    char _tab_version[8];
    char _tab_end_offset[8];
    char _tab_cols_num[8];
    CDBDiskTableColumn _tab_cols[(DISK_PGSIZE - 24) / sizeof(CDBDiskTableColumn)];
#elif __MACHINE_WORD_SIZE__ == 32
    char _tab_version[4];
    char _tab_end_offset[4];
    char _tab_cols_num[4];
    CDBDiskTableColumn _tab_cols[(DISK_PGSIZE - 12) / sizeof(CDBDiskTableColumn)];
#endif
}CDBDiskTableHeader;

#endif //TABLE_DEF_H
