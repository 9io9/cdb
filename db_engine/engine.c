//
// Created by mario on 2024/9/3.
//

#include <stdio.h>
#include <string.h>

#include "../db_parser/parser.def.h"

#include "table.def.h"
#include "engine.err.h"

#define __ENGINE_ERR__(e) CDBEngine ## e ## Error
#define __ENGINE_SUCCESS__ CDBEngineSuccess

inline char* crt_tab_name(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name->_cstr_ref;
}

inline size_t crt_tab_name_len(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name->_cstr_ref_len;
}

inline size_t crt_tab_cols_size(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz;
}

inline size_t* crt_tab_cols_size_ptr(const CDBCmd* cmd) {
    return &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz;
}

inline CDBTableColumn* crt_tab_cols(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla;
}

CDBEngineStatusCode exec_command(CDBCmd* cmd) {
    if (cmd == NULL) {
        return __ENGINE_ERR__(FuncArg);
    }

    switch (cmd->_cdb_cmd_type) {
        case Create:
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case Table:
                    char last_ch = crt_tab_name(cmd)[crt_tab_name_len(cmd)];

                    crt_tab_name(cmd)[crt_tab_name_len(cmd)] = '\0';

                    FILE* tab_file = fopen(crt_tab_name(cmd), "r");

                    if (tab_file != NULL) {
                        crt_tab_name(cmd)[crt_tab_name_len(cmd)] = last_ch;
                        fclose(tab_file);
                        return __ENGINE_ERR__(CrtDupTab);
                    }

                    tab_file = fopen(crt_tab_name(cmd), "wb+");

                    crt_tab_name(cmd)[crt_tab_name_len(cmd)] = last_ch;

                    size_t tab_end = DISK_PGSIZE;

                    CDBDiskTableHeader disk_table_header = { 0 };

                    memcpy(disk_table_header._tab_cols_num, crt_tab_cols_size_ptr(cmd), sizeof(size_t));

                    for (size_t col_id = 0; col_id < crt_tab_cols_size(cmd); ++col_id) {

                        memcpy(
                            disk_table_header._tab_cols[col_id]._col_type,
                            &crt_tab_cols(cmd)[col_id]._col_type,
                            sizeof(CDBTableColumnType)
                            );

                        if (crt_tab_cols(cmd)[col_id]._col_name->_cstr_ref_len > sizeof(disk_table_header._tab_cols[col_id]._col_name)) {
                            return __ENGINE_ERR__(CrtTabNameTooLong);
                        }

                        memcpy(
                            disk_table_header._tab_cols[col_id]._col_name,
                            crt_tab_cols(cmd)[col_id]._col_name->_cstr_ref,
                            crt_tab_cols(cmd)[col_id]._col_name->_cstr_ref_len
                            );
                        memcpy(
                            disk_table_header._tab_cols[col_id]._col_offset,
                            &tab_end,
                            sizeof(size_t)
                            );

                        tab_end += DISK_PGSIZE;
                    }

                    break;
                case DataBase:
                    break;

                default:
                    return __ENGINE_ERR__(CrtCmdType);
            }
            break;
        case Insert:
            break;
        case Modify:
            break;
        case Remove:
            break;
        case Select:
            break;

        default:
            return __ENGINE_ERR__(CmdType);
    }

    return __ENGINE_SUCCESS__;
}