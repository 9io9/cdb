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

CDBEngineStatusCode tabmeta_m2d(const char* tab_fname, const char* tabcol_fname, CDBTableColumn* cols, size_t cols_sz) {
    if (tab_fname == NULL || tabcol_fname == NULL || cols == NULL || cols_sz == 0) {
        return __ENGINE_ERR__(FuncArg);
    }

    if (access(tab_fname, F_OK) == 0 || access(tabcol_fname, F_OK) == 0) {
        return __ENGINE_ERR__(CrtDupTab);
    }

    time_t time_stamp = time(NULL);
    size_t col_offset = DISKPG_SIZE;
    CDBFileType ftype = CDBTableFile;
    CDBDiskTableHeader disk_tabhdr = { 0 };

    CDBDiskTableColumn* disk_cols = malloc(sizeof(*disk_cols) * cols_sz);

    memcpy(disk_tabhdr._file_type, &ftype, sizeof(CDBFileType));
    memcpy(disk_tabhdr._tab_cols_num, &cols_sz, sizeof(size_t));
    memcpy(disk_tabhdr._tab_crt_timestamp, &time_stamp, sizeof(time_t));

    for (size_t col_id = 0; col_id < cols_sz; ++col_id) {
        unsigned char col_name_len = cols[col_id]._col_name->_cstr_ref_len;

        memcpy(disk_cols[col_id]._col_type, &cols[col_id]._col_type, sizeof(CDBTableColumnType));
        memcpy(disk_cols[col_id]._col_name, cols[col_id]._col_name->_cstr_ref, cols[col_id]._col_name->_cstr_ref_len);
        memcpy(disk_cols[col_id]._col_name_len, &col_name_len, sizeof(unsigned char));
        memcpy(disk_cols[col_id]._col_offset, &col_offset, sizeof(size_t));

        // will not overflow, col_name's len in disk equals actual len + 1
        disk_cols[col_id]._col_name[cols[col_id]._col_name->_cstr_ref_len] = '\0';
        col_offset += DISKPG_SIZE;
    }

    FILE* tab_file = fopen(tab_fname, "wb+");
    FILE* col_file = fopen(tabcol_fname, "wb+");

    fwrite(&disk_tabhdr, sizeof(CDBDiskTableHeader), 1, tab_file);
    fwrite(disk_cols, sizeof(CDBDiskTableColumn), cols_sz, col_file);

    fflush(tab_file);
    fflush(col_file);

    free(disk_cols);
    fclose(tab_file);
    fclose(col_file);

    return __ENGINE_SUCCESS__;
}

CDBEngineStatusCode tabmeta_m2d_nocheck(const char* tab_fname, const char* tabcol_fname, CDBTableColumn* cols, size_t cols_sz) {
    if (access(tab_fname, F_OK) == 0 || access(tabcol_fname, F_OK) == 0) {
        return __ENGINE_ERR__(CrtDupTab);
    }

    time_t time_stamp = time(NULL);
    size_t col_offset = DISKPG_SIZE;
    CDBFileType ftype = CDBTableFile;
    CDBDiskTableHeader disk_tabhdr = { 0 };

    CDBDiskTableColumn* disk_cols = malloc(sizeof(*disk_cols) * cols_sz);

    memcpy(disk_tabhdr._file_type, &ftype, sizeof(CDBFileType));
    memcpy(disk_tabhdr._tab_cols_num, &cols_sz, sizeof(size_t));
    memcpy(disk_tabhdr._tab_crt_timestamp, &time_stamp, sizeof(time_t));

    for (size_t col_id = 0; col_id < cols_sz; ++col_id) {
        unsigned char col_name_len = cols[col_id]._col_name->_cstr_ref_len;

        memcpy(disk_cols[col_id]._col_type, &cols[col_id]._col_type, sizeof(CDBTableColumnType));
        memcpy(disk_cols[col_id]._col_name, cols[col_id]._col_name->_cstr_ref, cols[col_id]._col_name->_cstr_ref_len);
        memcpy(disk_cols[col_id]._col_name_len, &col_name_len, sizeof(unsigned char));
        memcpy(disk_cols[col_id]._col_offset, &col_offset, sizeof(size_t));

        // will not overflow, col_name's len in disk equals actual len + 1
        disk_cols[col_id]._col_name[cols[col_id]._col_name->_cstr_ref_len] = '\0';
        col_offset += DISKPG_SIZE;
    }

    FILE* tab_file = fopen(tab_fname, "wb+");
    FILE* col_file = fopen(tabcol_fname, "wb+");

    fwrite(&disk_tabhdr, sizeof(CDBDiskTableHeader), 1, tab_file);
    fwrite(disk_cols, sizeof(CDBDiskTableColumn), cols_sz, col_file);

    fflush(tab_file);
    fflush(col_file);

    free(disk_cols);
    fclose(tab_file);
    fclose(col_file);

    return __ENGINE_SUCCESS__;
}

static void crtcmd_fnames(char** tab_fname, char** col_fname, CDBCmd* cmd) {
    size_t name_buffer_len = (crtcmd_tabname_len(cmd) << 1) + sizeof(".columns") + 1;
    char* name_buffer = calloc(1, name_buffer_len);

    memcpy(name_buffer, crtcmd_tabname(cmd), crtcmd_tabname_len(cmd));
    name_buffer[crtcmd_tabname_len(cmd)] = '\0';
    memcpy(&name_buffer[crtcmd_tabname_len(cmd) + 1], crtcmd_tabname(cmd), crtcmd_tabname_len(cmd));
    memcpy(&name_buffer[(crtcmd_tabname_len(cmd) << 1) + 1], ".columns", sizeof(".columns") - 1);
    name_buffer[name_buffer_len - 1] = '\0';

    *tab_fname = name_buffer;
    *col_fname = &name_buffer[crtcmd_tabname_len(cmd) + 1];
}

CDBEngineStatusCode exec_command(CDBCmd* cmd) {
    if (cmd == NULL) {
        return __ENGINE_ERR__(FuncArg);
    }

    switch (cmd->_cdb_cmd_type) {
        case __CMD__(Create):
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case __HIER__(Table): {
                    char* tab_fname, *col_fname;

                    // get table file name, table columns file name
                    crtcmd_fnames(&tab_fname, &col_fname, cmd);

                    // dump table header and columns into table file and columns file
                    tabmeta_m2d_nocheck(tab_fname,col_fname,crtcmd_tabcols(cmd),crtcmd_tabcols_size(cmd));

                    fprintf(stdout, "Table %s Created\n", tab_fname);

                    free(tab_fname);
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