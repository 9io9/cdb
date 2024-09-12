//
// Created by mario on 2024/9/1.
//

#include <stdbool.h>

#include "../third_party/cvla/cvla.h"
#include "../third_party/cstr/cstr.h"

#include "../db_engine/table.def.h"
#include "../db_apis/name.h"
#include "../db_apis/access_func.h"

#include "parser.err.h"
#include "parser.def.h"

CDBParserStatusCode parse_col_type(CStringRef* token, CDBTableColumnType* col_type) {
    if (token == NULL || col_type == NULL) {
        return __PARSER_ERR__(FuncArg);
    }

    int cmp_res;

    if (cstr_ref_cmp_str_nocheck(token, "int", sizeof("int") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Int;
            return __PARSER_SUCCESS__;
        }
    }

    if (cstr_ref_cmp_str_nocheck(token, "str", sizeof("str") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Str;
            return __PARSER_SUCCESS__;
        }
    }

    return __PARSER_ERR__(TabColType);
}

CDBParserStatusCode parse_col_type_nocheck(CStringRef* token, CDBTableColumnType* col_type) {
    int cmp_res;

    if (cstr_ref_cmp_str_nocheck(token, "int", sizeof("int") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Int;
            return __PARSER_SUCCESS__;
        }
    }

    if (cstr_ref_cmp_str_nocheck(token, "str", sizeof("str") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Str;
            return __PARSER_SUCCESS__;
        }
    }

    return __PARSER_ERR__(TabColType);
}

CDBParserStatusCode parse_tab_row(CStringRef* tokens, size_t tokens_size, size_t* token_id, CVLArray** row) {
    if (tokens == NULL || tokens_size == 0 || token_id == NULL || row == NULL) {
        return __PARSER_ERR__(FuncArg);
    }

    bool has_row_before = false;
    CDBTableRow tab_row = { 0 };

    cvla_new_nocheck(0, sizeof(CDBTableRow), NULL, row);

    while (*token_id < tokens_size) {
        if (tokens[*token_id]._cstr_ref[0] == ')') {
            *token_id += 1;
            break;
        }

        if (tokens[*token_id]._cstr_ref[0] == ',') {
            if (!has_row_before) {
                return __PARSER_ERR__(SepFmt);
            }

            has_row_before = false;
        } else {
            if (*token_id + 2 >= tokens_size) {
                return __PARSER_ERR__(InsRowFmt);
            }

            if (tokens[*token_id + 1]._cstr_ref[0] != ':') {
                return __PARSER_ERR__(InsRowFmt);
            }

            CDBTableColumnType col_type;

            if (parse_col_type_nocheck(&tokens[*token_id + 2], &col_type) == __PARSER_ERR__(TabColType)) {
                return __PARSER_ERR__(TabColType);
            }

            switch (col_type) {
                case Int:
                    if (cstr_ref_cast_to_ssize_nocheck(&tokens[*token_id], &tab_row._row._row_int, 10) != CStrCastSuccess) {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    tab_row._row_type = Int;
                    break;
                case Str:
                    if (tokens[*token_id]._cstr_ref[0] != '\'') {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    if (tokens[*token_id]._cstr_ref[tokens[*token_id]._cstr_ref_len - 1] != '\'') {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    if (tokens[*token_id]._cstr_ref_len == 2) {
                        tab_row._row._row_str->_cstr_ref_len = 0;
                        break;
                    }

                    tab_row._row._row_str->_cstr_ref = &tokens[*token_id]._cstr_ref[1];
                    tab_row._row._row_str->_cstr_ref_len = tokens[*token_id]._cstr_ref_len - 2;

                    tab_row._row_type = Str;
                    break;
                case UInt:
                    if (cstr_ref_cast_to_size_nocheck(&tokens[*token_id], &tab_row._row._row_uint, 10) != CStrCastSuccess) {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    tab_row._row_type = UInt;
                    break;
                default:
                    return __PARSER_ERR__(TabColType);
            }

            *token_id += 2;
            has_row_before = true;
        }

        *token_id += 1;
    }

    return __PARSER_SUCCESS__;
}

CDBParserStatusCode parse_tab_row_nocheck(CStringRef* tokens, size_t tokens_size, size_t* token_id, CVLArray** row) {
    bool has_row_before = false;
    CDBTableRow tab_row = { 0 };

    cvla_new_nocheck(0, sizeof(CDBTableRow), NULL, row);

    while (*token_id < tokens_size) {
        if (tokens[*token_id]._cstr_ref[0] == ')') {
            *token_id += 1;
            break;
        }

        if (tokens[*token_id]._cstr_ref[0] == ',') {
            if (!has_row_before) {
                return __PARSER_ERR__(SepFmt);
            }

            has_row_before = false;
        } else {
            if (*token_id + 2 >= tokens_size) {
                return __PARSER_ERR__(InsRowFmt);
            }

            if (tokens[*token_id + 1]._cstr_ref[0] != ':') {
                return __PARSER_ERR__(InsRowFmt);
            }

            CDBTableColumnType col_type;

            if (parse_col_type_nocheck(&tokens[*token_id + 2], &col_type) == __PARSER_ERR__(TabColType)) {
                return __PARSER_ERR__(TabColType);
            }

            switch (col_type) {
                case Int:
                    if (cstr_ref_cast_to_ssize_nocheck(&tokens[*token_id], &tab_row._row._row_int, 10) != CStrCastSuccess) {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    tab_row._row_type = Int;
                    break;
                case Str:
                    if (tokens[*token_id]._cstr_ref[0] != '\'') {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    if (tokens[*token_id]._cstr_ref[tokens[*token_id]._cstr_ref_len - 1] != '\'') {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    if (tokens[*token_id]._cstr_ref_len == 2) {
                        tab_row._row._row_str->_cstr_ref_len = 0;
                        break;
                    }

                    tab_row._row._row_str->_cstr_ref = &tokens[*token_id]._cstr_ref[1];
                    tab_row._row._row_str->_cstr_ref_len = tokens[*token_id]._cstr_ref_len - 2;

                    tab_row._row_type = Str;
                break;
                case UInt:
                    if (cstr_ref_cast_to_size_nocheck(&tokens[*token_id], &tab_row._row._row_uint, 10) != CStrCastSuccess) {
                        return __PARSER_ERR__(InsRowValue);
                    }

                    tab_row._row_type = UInt;
                    break;
                default:
                    return __PARSER_ERR__(TabColType);
            }

            *token_id += 2;
            has_row_before = true;
        }

        *token_id += 1;
    }

    return __PARSER_SUCCESS__;
}

CDBParserStatusCode parse_tab_cols(CStringRef* tokens, size_t tokens_size, size_t* token_id, CVLArray** cols) {
    if (tokens == NULL || tokens_size == 0 || token_id == NULL || cols == NULL) {
        return __PARSER_ERR__(FuncArg);
    }

    bool has_col_before = false;
    CDBTableColumn col = { 0 };

    cvla_new_nocheck(0, sizeof(CDBTableColumn), NULL, cols);

    while (*token_id < tokens_size) {
        if (tokens[*token_id]._cstr_ref[0] == ')') {
            *token_id += 1;
            break;
        }

        if (tokens[*token_id]._cstr_ref[0] == ',') {
            if (!has_col_before) {
                return __PARSER_ERR__(SepFmt);
            }

            has_col_before = false;
        } else {
            if (*token_id + 2 >= tokens_size) {
                return __PARSER_ERR__(TabColFmt);
            }

            col._col_name = &tokens[*token_id];

            if (col._col_name->_cstr_ref_len > TABCOL_NAME_MAX) {
                return __PARSER_ERR__(TabColNameLen);
            }

            if (tokens[*token_id + 1]._cstr_ref[0] != ':') {
                return __PARSER_ERR__(TabColFmt);
            }

            if (parse_col_type_nocheck(&tokens[*token_id + 2], &col._col_type) == __PARSER_ERR__(TabColType)) {
                return __PARSER_ERR__(TabColType);
            }

            cvla_push_nocheck(&col, *cols);

            *token_id += 2;
            has_col_before = true;
        }

        *token_id += 1;
    }

    return __PARSER_SUCCESS__;
}

CDBParserStatusCode parse_tab_cols_nocheck(CStringRef* tokens, size_t tokens_size, size_t* token_id, CVLArray** cols) {
    bool has_col_before = false;
    CDBTableColumn col = { 0 };

    cvla_new_nocheck(0, sizeof(CDBTableColumn), NULL, cols);

    while (*token_id < tokens_size) {
        if (tokens[*token_id]._cstr_ref[0] == ')') {
            *token_id += 1;
            break;
        }

        if (tokens[*token_id]._cstr_ref[0] == ',') {
            if (!has_col_before) {
                cvla_free_nocheck(cols);
                return __PARSER_ERR__(SepFmt);
            }

            has_col_before = false;
        } else {
            if (*token_id + 2 >= tokens_size) {
                cvla_free_nocheck(cols);
                return __PARSER_ERR__(TabColFmt);
            }

            col._col_name = &tokens[*token_id];

            if (col._col_name->_cstr_ref_len > TABCOL_NAME_MAX) {
                return __PARSER_ERR__(TabColNameLen);
            }

            if (tokens[*token_id + 1]._cstr_ref[0] != ':') {
                cvla_free_nocheck(cols);
                return __PARSER_ERR__(TabColFmt);
            }

            if (parse_col_type_nocheck(&tokens[*token_id + 2], &col._col_type) == __PARSER_ERR__(TabColType)) {
                cvla_free_nocheck(cols);
                return __PARSER_ERR__(TabColType);
            }

            cvla_push_nocheck(&col, *cols);

            *token_id += 2;
            has_col_before = true;
        }

        *token_id += 1;
    }

    return __PARSER_SUCCESS__;
}

CDBParserStatusCode parse_tokens(CStringRef* tokens, size_t tokens_size, CDBCmd* cmd) {
    if (tokens == NULL || tokens_size == 0 || cmd == NULL) {
        return __PARSER_ERR__(FuncArg);
    }

    int token_cmp_res;
    size_t token_id = 0;

    if (cstr_ref_cmp_str_nocheck(tokens, "create", sizeof("create") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id + 1 < tokens_size) {
            token_id += 1;
            cmd->_cdb_cmd_type = __CMD__(Create);

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id + 2 < tokens_size) {
                    cmd->_cdb_cmd._crt_cmd._crt_type = __HIER__(Table);
                    crtcmd_set_tabname(cmd, &tokens[++token_id]);

                    if (tokens[++token_id]._cstr_ref[0] != '(') {
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    token_id += 1;

                    CDBParserStatusCode col_status = parse_tab_cols_nocheck(tokens, tokens_size, &token_id, crtcmd_tabcols_cvla_ref(cmd));

                    if (col_status != __PARSER_SUCCESS__) {
                        cvla_free_nocheck(crtcmd_tabcols_cvla_ref(cmd));
                        return col_status;
                    }

                    if (crtcmd_tabcols_size(cmd) == 0) {
                        cvla_free_nocheck(crtcmd_tabcols_cvla_ref(cmd));
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    if (token_id < tokens_size) {
                        cvla_free_nocheck(crtcmd_tabcols_cvla_ref(cmd));
                        return __PARSER_ERR__(CmdFmt);
                    }

                    return __PARSER_SUCCESS__;
                }
            }
        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "insert", sizeof("insert") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id + 1 < tokens_size) {
            token_id += 1;
            cmd->_cdb_cmd_type = __CMD__(Insert);

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "row", sizeof("row") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id + 2 < tokens_size) {
                    cmd->_cdb_cmd._insert_cmd._insert_type = __ATTR__(TableRows);
                    cmd->_cdb_cmd._insert_cmd._insert_name = &tokens[++token_id];

                    if (tokens[++token_id]._cstr_ref[0] != '(') {
                        return __PARSER_ERR__(InsRowFmt);
                    }

                    token_id += 1;

                    CDBParserStatusCode row_status = parse_tab_row_nocheck(tokens, tokens_size, &token_id, &cmd->_cdb_cmd._insert_cmd._insert_row);

                    if (row_status != __PARSER_SUCCESS__) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._insert_cmd._insert_row);
                        return row_status;
                    }

                    if (inscmd_rows_size(cmd) == 0) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._insert_cmd._insert_row);
                        return __PARSER_ERR__(InsRowFmt);
                    }

                    if (token_id < tokens_size) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._insert_cmd._insert_row);
                        return __PARSER_ERR__(CmdFmt);
                    }

                    return __PARSER_SUCCESS__;
                }
            } else if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "column", sizeof("column") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id + 2 < tokens_size) {

                }
            } else {
                return __PARSER_ERR__(InsType);
            }
        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "remove", sizeof("remove") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0) {

        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "modify", sizeof("modify") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0) {

        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "show", sizeof("show") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id + 1 < tokens_size) {
            token_id += 1;
            cmd->_cdb_cmd_type = __CMD__(Show);

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id + 2 < tokens_size) {
                    cmd->_cdb_cmd._show_cmd._show_type = __HIER__(Table);
                    cmd->_cdb_cmd._show_cmd._name = &tokens[++token_id];

                    token_id += 1;

                    if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "columns", sizeof("columns") - 1, &token_cmp_res) == CStrOpSuccess) {
                        if (token_cmp_res == 0) {
                            cmd->_cdb_cmd._show_cmd._show_detail_type = __ATTR__(TableColumns);
                            return __PARSER_SUCCESS__;
                        }
                    } else if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "rows", sizeof("rows") - 1, &token_cmp_res) == CStrOpSuccess) {
                        if (token_cmp_res == 0) {
                            cmd->_cdb_cmd._show_cmd._show_detail_type = __ATTR__(TableRows);
                            return __PARSER_SUCCESS__;
                        }
                    } else {
                        return __PARSER_ERR__(ShowCmdAttrType);
                    }
                }
            }
        }
    }

    return __PARSER_ERR__(CmdFmt);
}

CDBParserStatusCode parse_tokens_nocheck(CStringRef* tokens, size_t tokens_size, CDBCmd* cmd) {
    int token_cmp_res;
    size_t token_id = 0;

    if (cstr_ref_cmp_str_nocheck(tokens, "create", sizeof("create") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id + 1 < tokens_size) {
            token_id += 1;
            cmd->_cdb_cmd_type = __CMD__(Create);

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id + 2 < tokens_size) {
                    cmd->_cdb_cmd._crt_cmd._crt_type = __HIER__(Table);
                    crtcmd_set_tabname(cmd, &tokens[++token_id]);

                    if (tokens[++token_id]._cstr_ref[0] != '(') {
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    token_id += 1;

                    CDBParserStatusCode col_status = parse_tab_cols_nocheck(tokens, tokens_size, &token_id, crtcmd_tabcols_cvla_ref(cmd));

                    if (col_status != __PARSER_SUCCESS__) {
                        return col_status;
                    }

                    if (crtcmd_tabcols_size(cmd) == 0) {
                        cvla_free_nocheck(crtcmd_tabcols_cvla_ref(cmd));
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    if (token_id < tokens_size) {
                        cvla_free_nocheck(crtcmd_tabcols_cvla_ref(cmd));
                        return __PARSER_ERR__(CmdFmt);
                    }

                    return __PARSER_SUCCESS__;
                }
            }
        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "insert", sizeof("insert") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0) {

        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "remove", sizeof("remove") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0) {

        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "modify", sizeof("modify") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0) {

        }
    }

    if (cstr_ref_cmp_str_nocheck(tokens, "show", sizeof("show") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id + 1 < tokens_size) {
            token_id += 1;
            cmd->_cdb_cmd_type = __CMD__(Show);

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id + 2 < tokens_size) {
                    cmd->_cdb_cmd._show_cmd._show_type = __HIER__(Table);
                    cmd->_cdb_cmd._show_cmd._name = &tokens[++token_id];

                    token_id += 1;

                    if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "columns", sizeof("columns") - 1, &token_cmp_res) == CStrOpSuccess) {
                        if (token_cmp_res == 0) {
                            cmd->_cdb_cmd._show_cmd._show_detail_type = __ATTR__(TableColumns);
                            return __PARSER_SUCCESS__;
                        }
                    } else if (cstr_ref_cmp_str_nocheck(&tokens[token_id], "rows", sizeof("rows") - 1, &token_cmp_res) == CStrOpSuccess) {
                        if (token_cmp_res == 0) {
                            cmd->_cdb_cmd._show_cmd._show_detail_type = __ATTR__(TableRows);
                            return __PARSER_SUCCESS__;
                        }
                    } else {
                        return __PARSER_ERR__(ShowCmdAttrType);
                    }
                }
            }
        }
    }

    return __PARSER_ERR__(CmdFmt);
}