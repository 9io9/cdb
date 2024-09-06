//
// Created by mario on 2024/9/3.
//

#include <stdio.h>

#include "../third_party/cstr/cstr.def.h"

#include "../db_engine/table.def.h"
#include "../db_parser/parser.def.h"
#include "../db_apis/name.h"

#include "display.err.h"

static const char* COL_TYPE_STRS[] = {
    "Int",
    "CArr",
};

CDBDisplayStatusCode kv_ref_fprint(FILE* f, const char* key, CStringRef* value) {
    if (f == NULL || key == NULL || value == NULL) {
        return __DISP_ERR__(FuncArg);
    }

    fprintf(f, "%s: ", key);
    fwrite(value->_cstr_ref, sizeof(char), value->_cstr_ref_len, f);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode kv_str_fprint(FILE* f, const char* key, const char* value) {
    if (f == NULL || key == NULL || value == NULL) {
        return __DISP_ERR__(FuncArg);
    }

    fprintf(f, "%s: %s", key, value);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode title_fprint(FILE* f, const char* title, const char tsep) {
    if (f == NULL || title == NULL) {
        return __DISP_ERR__(FuncArg);
    }

    fprintf(f, "%s %c ", title, tsep);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode end_fprint(FILE* f, const char tsep) {
    if (f == NULL) {
        return __DISP_ERR__(FuncArg);
    }

    fprintf(f, " %c\n", tsep);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode kv_tabcols_fprint(FILE* f, const char* key, CDBTableColumn* cols, size_t cols_size) {
    if (f == NULL || key == NULL || cols == NULL || cols_size == 0) {
        return __DISP_ERR__(FuncArg);
    }

    fprintf(f, "%s: [", key);

    for (size_t col_id = 0; col_id < cols_size; ++col_id) {
        kv_ref_fprint(f, COL_TYPE_STRS[cols[col_id]._col_type], cols[col_id]._col_name);
        fprintf(f, ", ");
    }

    fprintf(f, "\b\b]");
}

CDBDisplayStatusCode kv_tabcol_fprint(FILE* f, CDBTableColumn* col) {
    if (f == NULL || col == NULL) {
        return __DISP_ERR__(FuncArg);
    }

    kv_ref_fprint(f, COL_TYPE_STRS[col->_col_type], col->_col_name);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode cmd_fprint(FILE* f, CDBCmd* cmd) {
    if (f == NULL || cmd == NULL) {
        return __DISP_ERR__(FuncArg);
    }

    switch (cmd->_cdb_cmd_type) {
        case __CMD__(Create):
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case __HIER__(Table):
                    title_fprint(f, "Command", '{');
                kv_str_fprint(f, "cmd_type", "Create");
                fprintf(f, ", ");
                kv_str_fprint(f, "crt_type", "Table");
                fprintf(f, ", ");
                kv_ref_fprint(f, "tab_name", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name);
                fprintf(f, ", ");
                kv_tabcols_fprint(f, "tab_cols", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla, cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz);
                end_fprint(f, '}');
                break;
                case __HIER__(DataBase):
                    break;
                default:
                    return __DISP_ERR__(CmdType);
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
            title_fprint(f, "Show", '{');
            kv_ref_fprint(f, "name", cmd->_cdb_cmd._show_cmd._name);
            fprintf(f, ", ");

            switch (cmd->_cdb_cmd._show_cmd._show_type) {
                case __HIER__(Table):
                    kv_str_fprint(f, "hier", "Table");
                    break;
                case __HIER__(DataBase):
                    kv_str_fprint(f, "hier", "DataBase");
                    break;
                default:
                    return __DISP_ERR__(CmdType);
            }

            fprintf(f, ", ");

            switch (cmd->_cdb_cmd._show_cmd._show_detail_type) {
                case __ATTR__(TableColumns):
                    kv_str_fprint(f, "attr", "Columns");
                    break;
                case __ATTR__(TableRows):
                    kv_str_fprint(f, "attr", "Rows");
                    break;
                default:
                    return __DISP_ERR__(CmdType);
            }

            end_fprint(f, '}');
            break;
        default:
            return __DISP_ERR__(CmdType);
    }

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode kv_ref_fprint_nocheck(FILE* f, const char* key, CStringRef* value) {
    fprintf(f, "%s: ", key);
    fwrite(value->_cstr_ref, sizeof(char), value->_cstr_ref_len, f);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode kv_str_fprint_nocheck(FILE* f, const char* key, const char* value) {
    fprintf(f, "%s: %s", key, value);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode title_fprint_nocheck(FILE* f, const char* title, const char tsep) {
    fprintf(f, "%s %c ", title, tsep);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode end_fprint_nocheck(FILE* f, const char tsep) {
    fprintf(f, " %c\n", tsep);

    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode kv_tabcols_fprint_nocheck(FILE* f, const char* key, CDBTableColumn* cols, size_t cols_size) {
    fprintf(f, "%s: [", key);

    for (size_t col_id = 0; col_id < cols_size; ++col_id) {
        kv_ref_fprint(f, COL_TYPE_STRS[cols[col_id]._col_type], cols[col_id]._col_name);
        fprintf(f, ", ");
    }

    fprintf(f, "\b\b]");
}

CDBDisplayStatusCode kv_tabcol_fprint_nocheck(FILE* f, CDBTableColumn* col) {
    kv_ref_fprint(f, COL_TYPE_STRS[col->_col_type], col->_col_name);
    return __DISP_SUCCESS__;
}

CDBDisplayStatusCode cmd_fprint_nocheck(FILE* f, CDBCmd* cmd) {
    switch (cmd->_cdb_cmd_type) {
        case __CMD__(Create):
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case __HIER__(Table):
                    title_fprint(f, "Command", '{');
                    kv_str_fprint(f, "cmd_type", "Create");
                    fprintf(f, ", ");
                    kv_str_fprint(f, "crt_type", "Table");
                    fprintf(f, ", ");
                    kv_ref_fprint(f, "tab_name", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name);
                    fprintf(f, ", ");
                    kv_tabcols_fprint(f, "tab_cols", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla, cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz);
                    end_fprint(f, '}');
                    break;
                case __HIER__(DataBase):
                    break;
                default:
                    return __DISP_ERR__(CmdType);
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
            title_fprint(f, "Show", '{');
            kv_ref_fprint(f, "name", cmd->_cdb_cmd._show_cmd._name);
            fprintf(f, ", ");

            switch (cmd->_cdb_cmd._show_cmd._show_type) {
                case __HIER__(Table):
                    kv_str_fprint(f, "hier", "Table");
                    break;
                case __HIER__(DataBase):
                    kv_str_fprint(f, "hier", "DataBase");
                    break;
                default:
                    return __DISP_ERR__(CmdType);
            }

            fprintf(f, ", ");

            switch (cmd->_cdb_cmd._show_cmd._show_detail_type) {
                case __ATTR__(TableColumns):
                    kv_str_fprint(f, "attr", "Columns");
                    break;
                case __ATTR__(TableRows):
                    kv_str_fprint(f, "attr", "Rows");
                    break;
                default:
                    return __DISP_ERR__(CmdType);
            }

            end_fprint(f, '}');
            break;
        default:
            return __DISP_ERR__(CmdType);
    }

    return __DISP_SUCCESS__;
}