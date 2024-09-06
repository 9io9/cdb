//
// Created by mario on 2024/9/1.
//

#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "../third_party/cstr/cstr.def.h"
#include "../third_party/cvla/cvla.def.h"

#define __NAME__(n) CDBCmd ## n

typedef enum CDBCmdType {
    __NAME__(Create) = 0,
    __NAME__(Select),
    __NAME__(Insert),
    __NAME__(Remove),
    __NAME__(Modify),
    __NAME__(Show)
}CDBCmdType;

#undef __NAME__

#define __NAME__(n) CDBHier ## n

typedef enum CDBHierachyType {
    __NAME__(Table) = 0,
    __NAME__(DataBase),
}CDBHierachyType;

#undef __NAME__

#define __NAME__(n) CDBAttr ## n

typedef enum CDBAttrType {
    __NAME__(TableColumns) = 0,
    __NAME__(TableRows)
}CDBAttrType;

#undef __NAME__

typedef struct CDBCreateCmd {
    CDBHierachyType _crt_type;
    union {
        struct CreateTableCommand {
            CStringRef* _tab_name;
            CVLArray* _tab_cols;
        }_tab;
    }_cmd;
}CDBCreateCmd;

typedef struct CDBShowCmd {
    CDBHierachyType _show_type;
    CDBAttrType _show_detail_type;
    CStringRef* _name;
}CDBShowCmd;

typedef struct CDBCmd {
    CDBCmdType _cdb_cmd_type;
    union {
        CDBCreateCmd _crt_cmd;
        CDBShowCmd _show_cmd;
    }_cdb_cmd;
}CDBCmd;

#endif //PARSER_DEF_H
