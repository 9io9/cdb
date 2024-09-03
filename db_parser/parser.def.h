//
// Created by mario on 2024/9/1.
//

#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "../third_party/cstr/cstr.def.h"
#include "../third_party/cvla/cvla.def.h"

#define FUNC_ASSERT(...)

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

#endif //PARSER_DEF_H
