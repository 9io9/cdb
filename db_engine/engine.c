//
// Created by mario on 2024/9/3.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../third_party/cvla/cvla.h"

#include "../db_parser/parser.def.h"
#include "../db_apis/name.h"
#include "../db_apis/access_func.h"
#include "../db_display/display.h"

#include "table.def.h"
#include "engine.def.h"
#include "engine.err.h"


CDBEngineStatusCode exec_command(CDBCmd* cmd) {
    if (cmd == NULL) {
        return __ENGINE_ERR__(FuncArg);
    }

    switch (cmd->_cdb_cmd_type) {
        case __CMD__(Create):
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case __HIER__(Table): {
                    char* tabcol_file_name = calloc(1, crtcmd_tabname_len(cmd) + sizeof(".columns"));

                    memcpy(tabcol_file_name, crtcmd_tabname(cmd), crtcmd_tabname_len(cmd));

                    if (access(tabcol_file_name, F_OK) == 0) {
                        free(tabcol_file_name);
                        return __ENGINE_ERR__(CrtDupTab);
                    }

                    FILE* tab_file = fopen(tabcol_file_name, "wb+");

                    memcpy(&tabcol_file_name[crtcmd_tabname_len(cmd)], ".columns", sizeof(".columns") - 1);

                    if (access(tabcol_file_name, F_OK) == 0) {
                        free(tabcol_file_name);
                        fclose(tab_file);
                        return __ENGINE_ERR__(CrtDupTab);
                    }

                    FILE* col_file = fopen(tabcol_file_name, "wb+");

                    time_t timestamp;
                    size_t end_offset = DISKPG_SIZE;
                    CDBFileType file_type = CDBTableFile;
                    CDBDiskTableHeader disk_table_header = { 0 };

                    time(&timestamp);

                    memcpy(disk_table_header._tab_cols_num, crtcmd_tabcols_size_ref(cmd), sizeof(size_t));
                    memcpy(disk_table_header._file_type, &file_type, sizeof(CDBFileType));
                    memcpy(disk_table_header._tab_crt_timestamp, &timestamp, sizeof(time_t));

                    for (size_t col_id = 0; col_id < crtcmd_tabcols_size(cmd); ++col_id) {
                        CDBDiskTableColumn col;

                        memcpy(col._col_name, crtcmd_tabcols(cmd)[col_id]._col_name->_cstr_ref, crtcmd_tabcols(cmd)[col_id]._col_name->_cstr_ref_len);
                        col._col_name[TABCOL_NAME_MAX] = '\0';

                        memcpy(col._col_name_len, &crtcmd_tabcols(cmd)[col_id]._col_name->_cstr_ref_len, sizeof(size_t));
                        memcpy(col._col_type, &crtcmd_tabcols(cmd)[col_id]._col_type, sizeof(CDBTableColumnType));
                        memcpy(col._col_offset, &end_offset, sizeof(size_t));

                        fwrite(&col, sizeof(CDBDiskTableColumn), 1, col_file);
                        end_offset += DISKPG_SIZE;
                    }

                    memcpy(disk_table_header._tab_end_offset, &end_offset, sizeof(size_t));

                    fwrite(&disk_table_header, sizeof(char), sizeof(CDBDiskTableHeader), tab_file);

                    cvla_clean_nocheck(crtcmd_tabcols_cvla(cmd));
                    free(tabcol_file_name);
                    fflush(col_file);
                    fclose(col_file);
                    fflush(tab_file);
                    fclose(tab_file);

                    fprintf(stdout, "Table ");
                    fwrite(crtcmd_tabname(cmd), sizeof(char), crtcmd_tabname_len(cmd), stdout);
                    fprintf(stdout, " Created\n");
                    break;
                }
                case __HIER__(DataBase): {
                    break;
                }
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
                case __HIER__(Table): {
                    char* tabcol_file_name = calloc(1, showcmd_name_len(cmd) + sizeof(".columns"));

                    memcpy(tabcol_file_name, showcmd_name(cmd), showcmd_name_len(cmd));

                    if (access(tabcol_file_name, F_OK) != 0) {
                        free(tabcol_file_name);
                        return __ENGINE_ERR__(ShowTabNotExist);
                    }

                    FILE* tab_file = fopen(tabcol_file_name, "rb");

                    memcpy(&tabcol_file_name[showcmd_name_len(cmd)], ".columns", sizeof(".columns") - 1);

                    if (access(tabcol_file_name, F_OK) != 0) {
                        free(tabcol_file_name);
                        fclose(tab_file);
                        return __ENGINE_ERR__(ShowTabNotExist);
                    }

                    FILE* col_file = fopen(tabcol_file_name, "rb");

                    CDBDiskTableHeader disk_table_header = { 0 };

                    fread(&disk_table_header, sizeof(CDBDiskTableHeader), 1, tab_file);

                    switch (cmd->_cdb_cmd._show_cmd._show_detail_type) {
                        case __ATTR__(TableColumns): {
                            size_t col_num;
                            CStringRef col_name_ref = { 0 };
                            CDBTableColumn col = { ._col_name = &col_name_ref };

                            memcpy(&col_num, disk_table_header._tab_cols_num, sizeof(size_t));

                            fprintf(stdout, "Table ");
                            fwrite(showcmd_name(cmd), sizeof(char), showcmd_name_len(cmd), stdout);
                            fprintf(stdout, "'s Columns [");

                            for (size_t col_id = 0; col_id < col_num; ++col_id) {
                                unsigned char col_name_len;
                                CDBDiskTableColumn disk_col;

                                fread(&disk_col, sizeof(CDBDiskTableColumn), 1, col_file);

                                col._col_name->_cstr_ref = disk_col._col_name;
                                memcpy(&col_name_len, disk_col._col_name_len, sizeof(unsigned char));
                                col._col_name->_cstr_ref_len = col_name_len;

                                memcpy(&col._col_type, disk_col._col_type, sizeof(CDBTableColumnType));

                                kv_tabcol_fprint_nocheck(stdout, &col);
                                fprintf(stdout, ", ");
                            }

                            fprintf(stdout, "\b\b]\n");
                            break;
                        }
                        case __ATTR__(TableRows): {
                            break;
                        }
                        default:
                            return __ENGINE_ERR__(ShowCmdAttrType);
                    }

                    free(tabcol_file_name);
                    fclose(tab_file);
                    fclose(col_file);
                    break;
                }
                case __HIER__(DataBase): {
                    break;
                }
                default:
                    return __ENGINE_ERR__(ShowCmdType);
            }
            break;
        default:
            return __ENGINE_ERR__(CmdType);
    }

    return __ENGINE_SUCCESS__;
}