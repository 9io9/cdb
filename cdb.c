#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cstr/cstr.h"
#include "cvla/cvla.h"

#include "cdb.def.h"
#include "cdb.err.h"

static const char* col_type_strs[] = {
    "Int",
    "CArr",
};

static bool is_spec(char ch) {
    return ch == ',' || ch == '(' || ch == ')' || ch == ':';
}

static bool is_csep(char ch) {
    return ch == '_' || ch == '@' || ch == '$' || ch == '#';
}

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
    TableColumn* col = data;

    for (size_t col_id = 0; col_id < len; ++col_id) {
        kv_ref_fprint(f, col_type_strs[col[col_id]._col_type], col[col_id]._col_name);
        fprintf(f, ", ");
    }

    fprintf(f, "\b\b");
}

static void kv_tabcols_fprint(FILE* f, const char* key, CVLArray* cvla) {
    fprintf(f, "%s: [", key);

    cvla_fprint_nocheck(f, custom_tabcols_fprint_func, cvla);

    fprintf(f, "]");
}

static CDBStatusCode parse_col_type(CStringRef* token, CDBTableColumnType* col_type) {
    int cmp_res;

    if (cstr_ref_cmp_str_nocheck(token, "int", sizeof("int") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = Integer;
            return CDBSuccess;
        }
    }

    if (cstr_ref_cmp_str_nocheck(token, "charArr", sizeof("charArr") - 1, &cmp_res) == CStrOpSuccess) {
        if (cmp_res == 0) {
            *col_type = CharArray;
            return CDBSuccess;
        }
    }

    return CDBInvalidToken;
}

static CDBStatusCode cmd_fprint(FILE* f, CDBCmd* cmd) {
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
                    return CDBInvalidCmdType;
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
            return CDBInvalidCmdType;
    }
}

static CDBStatusCode read_line(FILE* f, CString* linebuffer) {
    char input_ch;

    while ((input_ch = fgetc(f)) != ';') {
        if (input_ch == ' ') {
            if (linebuffer->_cstr_sz != 0) {
                if (linebuffer->_cstr[linebuffer->_cstr_sz - 1] != ' ') {
                    cstr_pushc_nocheck(input_ch, linebuffer);
                }
            }
        } else if (is_spec(input_ch)) {
            if (linebuffer->_cstr_sz == 0) {
                cstr_pushc_nocheck(input_ch, linebuffer);
                cstr_pushc_nocheck(' ', linebuffer);
            } else if (linebuffer->_cstr[linebuffer->_cstr_sz - 1] != ' ') {
                cstr_pushc_nocheck(' ', linebuffer);
                cstr_pushc_nocheck(input_ch, linebuffer);
                cstr_pushc_nocheck(' ', linebuffer);
            } else {
                cstr_pushc_nocheck(input_ch, linebuffer);
                cstr_pushc_nocheck(' ', linebuffer);
            }
        } else if (input_ch == '\n') {
            fprintf(stdout, "cdb>");
        } else if (isalnum(input_ch)){
            cstr_pushc_nocheck(input_ch, linebuffer);
        } else {
            if (!is_csep(input_ch)) {
                fprintf(stderr, "CDB Error: Invalid Character\n");
                fflush(stdin);
                fprintf(stdout, "cbd>");
                return CDBInvalidCharacter;
            }
            cstr_pushc_nocheck(input_ch, linebuffer);
        }
    }

    fprintf(stdout, "cdb>");

    return CDBSuccess;
}

static CDBStatusCode parse_tokens(CStringRef* tokens, size_t tokens_size, CDBCmd* cmd) {
    int token_cmp_res;
    size_t token_id = 0;

    if (cstr_ref_cmp_str_nocheck(&tokens[token_id++], "create", sizeof("create") - 1, &token_cmp_res) == CStrOpSuccess) {
        if (token_cmp_res == 0 && token_id < tokens_size) {
            cmd->_cdb_cmd_type = Create;

            if (cstr_ref_cmp_str_nocheck(&tokens[token_id++], "table", sizeof("table") - 1, &token_cmp_res) == CStrOpSuccess) {
                if (token_cmp_res == 0 && token_id < tokens_size) {
                    cmd->_cdb_cmd._crt_cmd._crt_type = Table;
                    cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name = &tokens[token_id++];

                    cvla_new_nocheck(0, sizeof(TableColumn), NULL, &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);

                    if (tokens[token_id]._cstr_ref[0] != '(') {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                        return CDBInvalidToken;
                    }

                    token_id += 1;

                    bool has_col_before = false;
                    TableColumn col = { 0 };

                    while (token_id < tokens_size) {
                        if (tokens[token_id]._cstr_ref[0] == ')') {
                            token_id += 1;
                            break;
                        }

                        if (tokens[token_id]._cstr_ref[0] == ',') {
                            if (!has_col_before) {
                                cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                                return CDBInvalidSyntax;
                            }

                            has_col_before = false;
                        } else {
                            if (token_id + 2 >= tokens_size) {
                                cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                                return CDBInvalidSyntax;
                            }

                            col._col_name = &tokens[token_id];

                            if (tokens[token_id + 1]._cstr_ref[0] != ':') {
                                cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                                return CDBInvalidSyntax;
                            }

                            if (parse_col_type(&tokens[token_id + 2], &col._col_type) == CDBInvalidToken) {
                                cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                                return CDBInvalidToken;
                            }

                            cvla_push_nocheck(&col, cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);

                            token_id += 2;
                            has_col_before = true;
                        }

                        token_id += 1;
                    }


                    if (cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz == 0) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                        return CDBInvalidSyntax;
                    }

                    if (token_id < tokens_size) {
                        cvla_free_nocheck(&cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols);
                        return CDBInvalidToken;
                    }

                    return CDBSuccess;
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

    return CDBInvalidToken;
}

#ifdef __WCHAR_ENABLED__
static void cdb_main(int argc, wchar_t* argv[]) {

}
#else
static void cdb_main(int argc, char* argv[]) {
    size_t tokenbuffer_len = 0;
    CStringRef* tokenbuffer = NULL;
    CString* linebuffer = NULL;

    cstr_new_nocheck(0, NULL, &linebuffer);

    fprintf(stdout, "cdb>");

    while (true) {
        if (read_line(stdin, linebuffer) != CDBSuccess) {
            cstr_clean_nocheck(linebuffer);
            continue;
        }

        switch (cstr_split_nocheck(linebuffer, ' ', &tokenbuffer_len, &tokenbuffer)) {
            case CStrSuccess:
                CDBCmd cmd = { 0 };

                switch (parse_tokens(tokenbuffer, tokenbuffer_len, &cmd)) {
                    case CDBSuccess:
                        cmd_fprint(stdout, &cmd);
                        break;
                    default:
                        fprintf(stderr, "Invalid Command: Invalid Token Or Syntax\n");
                        break;
                }

                break;

            case CStrNoSplitError:
                int cmp_res;

                if (cstr_cmp_str_nocheck(linebuffer, "exit", sizeof("exit") - 1, &cmp_res) == CStrOpSuccess) {
                    if (cmp_res == 0) {
                        exit(CDBSuccess);
                    }
                }

                fprintf(stderr, "Invalid Command: Too few tokens\n");

                break;

            default:
                break;
        }

        cstr_clean_nocheck(linebuffer);
    }
}
#endif

void main(int argc, char* argv[])
{
    cdb_main(argc, argv);
}
