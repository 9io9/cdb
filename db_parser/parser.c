//
// Created by mario on 2024/9/1.
//

#include <stdbool.h>

#include "../third_party/cvla/cvla.h"
#include "../third_party/cstr/cstr.h"

#include "../db_engine/table.def.h"

#include "parser.err.h"
#include "parser.def.h"

#define __PARSER_ERR__(e) CDBParser ## e ## Error
#define __PARSER_SUCCESS__ CDBParserSuccess

static const char* COL_TYPE_STRS[] = {
    "Int",
    "CArr",
};

static void kv_ref_fprint(FILE* f, const char* key, CStringRef* value) {
    fprintf(f, "%s: ", key);
    fwrite(value->_cstr_ref, sizeof(char), value->_cstr_ref_len, f);
}

static void kv_str_fprint(FILE* f, const char* key, const char* value) {
    fprintf(f, "%s: %s", key, value);
}

static void title_fprint(FILE* f, const char* title, const char tsep) {
    fprintf(f, "%s %c ", title, tsep);
}

static void end_fprint(FILE* f, const char tsep) {
    fprintf(f, " %c\n", tsep);
}

static void custom_tabcols_fprint_func(FILE* f, void* data, size_t len) {
    CDBTableColumn* col = data;

    for (size_t col_id = 0; col_id < len; ++col_id) {
        kv_ref_fprint(f, COL_TYPE_STRS[col[col_id]._col_type], col[col_id]._col_name);
        fprintf(f, ", ");
    }

    fprintf(f, "\b\b");
}

static void kv_tabcols_fprint(FILE* f, const char* key, CVLArray* cvla) {
    fprintf(f, "%s: [", key);

    cvla_fprint_nocheck(f, custom_tabcols_fprint_func, cvla);

    fprintf(f, "]");
}

CDBParserStatusCode parse_col_type(CStringRef* token, CDBTableColumnType* col_type) {
    if (token == NULL || col_type == NULL) {
        return __PARSER_ERR__(FuncArg);
    }

    int cmp_res;

    if (cstr_ref_cmp_str_nocheck(token, "int", sizeof("int") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Integer;
            return __PARSER_SUCCESS__;
        }
    }

    if (cstr_ref_cmp_str_nocheck(token, "charArr", sizeof("charArr") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = CharArray;
            return __PARSER_SUCCESS__;
        }
    }

    return __PARSER_ERR__(TabColType);
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

            if (tokens[*token_id + 1]._cstr_ref[0] != ':') {
                cvla_free_nocheck(cols);
                return __PARSER_ERR__(TabColFmt);
            }

            if (parse_col_type(&tokens[*token_id + 2], &col._col_type) == __PARSER_ERR__(TabColType)) {
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

    if (cstr_ref_cmp_str_nocheck(&tokens[token_id++], "create", sizeof("create") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id < tokens_size) {
            cmd->_cdb_cmd_type = Create;

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id++], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id < tokens_size) {
                    cmd->_cdb_cmd._crt_cmd._crt_type = Table;
                    cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name = &tokens[token_id++];

                    if (tokens[token_id]._cstr_ref[0] != '(') {
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    token_id += 1;

                    parse_tab_cols(tokens, tokens_size, &token_id, &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);

                    if (cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz == 0) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    if (token_id < tokens_size) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
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

    return __PARSER_ERR__(CmdFmt);
}

CDBParserStatusCode cmd_fprint(FILE* f, CDBCmd* cmd) {
    if (f == NULL || cmd == NULL) {
        return __PARSER_ERR__(FuncArg);
    }

    switch (cmd->_cdb_cmd_type) {
        case Create:
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case Table:
                    title_fprint(f, "Command", '{');
                    kv_str_fprint(f, "cmd_type", "Create");
                    fprintf(f, ", ");
                    kv_str_fprint(f, "crt_type", "Table");
                    fprintf(f, ", ");
                    kv_ref_fprint(f, "tab_name", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name);
                    fprintf(f, ", ");
                    kv_tabcols_fprint(f, "tab_cols", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                    end_fprint(f, '}');
                    break;
                case DataBase:
                    break;
                default:
                    return __PARSER_ERR__(CrtCmdType);
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
            return __PARSER_ERR__(CmdType);
    }

    return __PARSER_SUCCESS__;
}

CDBParserStatusCode parse_col_type_nocheck(CStringRef* token, CDBTableColumnType* col_type) {
    int cmp_res;

    if (cstr_ref_cmp_str_nocheck(token, "int", sizeof("int") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Integer;
            return __PARSER_SUCCESS__;
        }
    }

    if (cstr_ref_cmp_str_nocheck(token, "charArr", sizeof("charArr") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = CharArray;
            return __PARSER_SUCCESS__;
        }
    }

    return __PARSER_ERR__(TabColType);
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

            if (tokens[*token_id + 1]._cstr_ref[0] != ':') {
                cvla_free_nocheck(cols);
                return __PARSER_ERR__(TabColFmt);
            }

            if (parse_col_type(&tokens[*token_id + 2], &col._col_type) == __PARSER_ERR__(TabColType)) {
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

CDBParserStatusCode parse_tokens_nocheck(CStringRef* tokens, size_t tokens_size, CDBCmd* cmd) {
    int token_cmp_res;
    size_t token_id = 0;

    if (cstr_ref_cmp_str_nocheck(&tokens[token_id++], "create", sizeof("create") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id < tokens_size) {
            cmd->_cdb_cmd_type = Create;

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id++], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id < tokens_size) {
                    cmd->_cdb_cmd._crt_cmd._crt_type = Table;
                    cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name = &tokens[token_id++];

                    if (tokens[token_id]._cstr_ref[0] != '(') {
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    token_id += 1;

                    parse_tab_cols(tokens, tokens_size, &token_id, &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);

                    if (cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz == 0) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                        return __PARSER_ERR__(CrtTabFmt);
                    }

                    if (token_id < tokens_size) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
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

    return __PARSER_ERR__(CmdFmt);
}

CDBParserStatusCode cmd_fprint_nocheck(FILE* f, CDBCmd* cmd) {
    switch (cmd->_cdb_cmd_type) {
        case Create:
            switch (cmd->_cdb_cmd._crt_cmd._crt_type) {
                case Table:
                    title_fprint(f, "Command", '{');
                    kv_str_fprint(f, "cmd_type", "Create");
                    fprintf(f, ", ");
                    kv_str_fprint(f, "crt_type", "Table");
                    fprintf(f, ", ");
                    kv_ref_fprint(f, "tab_name", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name);
                    fprintf(f, ", ");
                    kv_tabcols_fprint(f, "tab_cols", cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                    end_fprint(f, '}');
                    break;
                case DataBase:
                    break;
                default:
                    return __PARSER_ERR__(CrtCmdType);
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
            return __PARSER_ERR__(CmdType);
    }

    return __PARSER_SUCCESS__;
}