//
// Created by mario on 2024/9/3.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../third_party/cvla/cvla.h"

#include "../db_parser/parser.def.h"
#include "../db_apis/name.h"
#include "../db_display/display.h"

#include "table.def.h"
#include "engine.err.h"

static char* crt_tab_name(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name->_cstr_ref;
}

static size_t crt_tab_name_len(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name->_cstr_ref_len;
}

static size_t crt_tab_cols_size(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz;
}

static size_t* crt_tab_cols_size_ptr(const CDBCmd* cmd) {
    return &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz;
}

static CDBTableColumn* crt_tab_cols(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla;
}

static char* show_tab_name(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._show_cmd._name->_cstr_ref;
}

static size_t show_tab_name_len(const CDBCmd* cmd) {
    return cmd->_cdb_cmd._show_cmd._name->_cstr_ref_len;
}

CDBEngineStatusCode exec_command(CDBCmd* cmd) {
    if (cmd == NULL) {
        return __ENGINE_ERR__(FuncArg);
    }

    switch (cmd->_cdb_cmd_type) {
        case __CMD__(Create):
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case __HIER__(Table):
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

                    CDBDiskTableHeader* disk_table_header = calloc(1, sizeof(CDBDiskTableHeader));

                    memcpy(disk_table_header->_tab_cols_num, crt_tab_cols_size_ptr(cmd), sizeof(size_t));

                    for (size_t col_id = 0; col_id < crt_tab_cols_size(cmd); ++col_id) {

                        memcpy(
                            disk_table_header->_tab_cols[col_id]._col_type,
                            &crt_tab_cols(cmd)[col_id]._col_type,
                            sizeof(CDBTableColumnType)
                            );

                        if (crt_tab_cols(cmd)[col_id]._col_name->_cstr_ref_len > sizeof(disk_table_header->_tab_cols[col_id]._col_name) - 1) {
                            free(disk_table_header);
                            return __ENGINE_ERR__(CrtTabNameTooLong);
                        }

                        memcpy(
                            disk_table_header->_tab_cols[col_id]._col_name,
                            crt_tab_cols(cmd)[col_id]._col_name->_cstr_ref,
                            crt_tab_cols(cmd)[col_id]._col_name->_cstr_ref_len
                            );
                        memcpy(
                            disk_table_header->_tab_cols[col_id]._col_offset,
                            &tab_end,
                            sizeof(size_t)
                            );

                        tab_end += DISK_PGSIZE;
                    }

                    fwrite(disk_table_header, sizeof(CDBDiskTableHeader), 1, tab_file);

                    cvla_clean_nocheck(cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                    free(disk_table_header);
                    fflush(tab_file);
                    fclose(tab_file);

                    fprintf(stdout, "Table ");
                    fwrite(crt_tab_name(cmd), sizeof(char), crt_tab_name_len(cmd), stdout);
                    fprintf(stdout, " Created\n");
                    break;
                case __HIER__(DataBase):
                    break;

                default:
                    return __ENGINE_ERR__(CrtCmdType);
            }
            break;
        case __CMD__(Insert):
            break;
        case __CMD__(Modify):
            break;
        case __CMD__(Remove):
            break;
        case __CMD__(Select):
            break;
        case __CMD__(Show):
            switch (cmd->_cdb_cmd._show_cmd._show_type) {
                case __HIER__(Table):
                    char last_ch = show_tab_name(cmd)[show_tab_name_len(cmd)];
                    show_tab_name(cmd)[show_tab_name_len(cmd)] = '\0';

                    FILE* tab_file = fopen(show_tab_name(cmd), "rb");

                    show_tab_name(cmd)[show_tab_name_len(cmd)] = last_ch;

                    if (tab_file == NULL) {
                        return __ENGINE_ERR__(ShowTabNotExist);
                    }

                    CDBDiskTableHeader* disk_table_header = calloc(1, sizeof(CDBDiskTableHeader));

                    fread(disk_table_header, sizeof(CDBDiskTableHeader), 1, tab_file);

                    switch (cmd->_cdb_cmd._show_cmd._show_detail_type) {
                        case __ATTR__(TableColumns):
                            size_t col_num;
                            CDBTableColumn col = { ._col_name = malloc(sizeof(CStringRef)) };

                            memcpy(&col_num, disk_table_header->_tab_cols_num, sizeof(size_t));

                            fprintf(stdout, "Table ");
                            fwrite(show_tab_name(cmd), sizeof(char), show_tab_name_len(cmd), stdout);
                            fprintf(stdout, "'s Columns [");

                            for (size_t col_id = 0; col_id < col_num; ++col_id) {
                                memcpy(&col._col_type, disk_table_header->_tab_cols[col_id]._col_type, sizeof(col._col_type));

                                col._col_name->_cstr_ref = disk_table_header->_tab_cols[col_id]._col_name;
                                col._col_name->_cstr_ref_len = strlen(disk_table_header->_tab_cols[col_id]._col_name);

                                kv_tabcol_fprint_nocheck(stdout, &col);
                                fprintf(stdout, ", ");
                            }

                            fprintf(stdout, "\b\b]\n");

                            free(col._col_name);
                            break;
                        case __ATTR__(TableRows):
                            break;
                        default:
                            return __ENGINE_ERR__(ShowCmdAttrType);
                    }

                    free(disk_table_header);
                    fclose(tab_file);
                    break;
                case __HIER__(DataBase):
                    break;
                default:
                    return __ENGINE_ERR__(ShowCmdType);
            }
            break;
        default:
            return __ENGINE_ERR__(CmdType);
    }

    return __ENGINE_SUCCESS__;
}