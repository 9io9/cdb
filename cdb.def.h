//
// Created by mario on 2024/8/28.
//

#ifndef CDB_DEF_H
#define CDB_DEF_H

typedef enum CDBTableColumnType {
    Integer = 0,
    CharArray
}CDBTableColumnType;

typedef struct TableColumn {
    CDBTableColumnType _col_type;
    size_t _col_offset;
    CStringRef* _col_name;
}TableColumn;

typedef enum CDBCmdType {
    Create = 0,
    Select,
    Insert,
    Remove,
    Modify,
}CDBCmdType;

typedef enum CDBCreateCmdType {
    Table = 0,
    DataBase
}CDBCreateCmdType;

typedef struct CDBCreateCmd {
    CDBCreateCmdType _crt_type;
    union {
        struct CreateTableCommand {
            CStringRef* _tab_name;
            CVLArray* _tab_cols;
        }_tab;
    }_cmd;
}CDBCreateCmd;

typedef struct CDBCmd {
    CDBCmdType _cdb_cmd_type;
    union {
        CDBCreateCmd _crt_cmd;
    }_cdb_cmd;
}CDBCmd;

#endif //CDB_DEF_H
