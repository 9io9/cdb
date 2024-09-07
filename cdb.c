#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "third_party/cstr/cstr.h"
#include "third_party/cprt/cprt.h"

#include "db_parser/parser.h"
#include "db_shell/shell.h"
#include "db_display/display.h"
#include "db_apis/name.h"
#include "db_engine/engine.h"

#include "cdb.err.h"

#define PERR(e) \
cprt_forecolor_style_start_nocheck(CPrtForeRed, stdout); \
fprintf(stdout, e "\n"); \
cprt_end_nocheck(stdout)

#ifdef __WCHAR_ENABLED__
static void cdb_main(int argc, wchar_t* argv[]) {

}
#else
static void cdb_main(int argc, char* argv[]) {
    size_t tokenbuffer_len = 0;
    CStringRef* tokenbuffer = NULL;
    CString* linebuffer = NULL;

    cstr_new_nocheck(0, NULL, &linebuffer);

    while (true) {
        switch (read_line(stdin, linebuffer)) {
            case __SHELL_ERR__(InvalidChar):
                PERR("Invalid Character: character from input is not in valid charset");
                cstr_clean_nocheck(linebuffer);
                continue;

            case __SHELL_SUCCESS__:
                break;

            default:
                break;
        }

        switch (cstr_split_nocheck(linebuffer, ' ', &tokenbuffer_len, &tokenbuffer)) {
            case CStrSuccess:
                CDBCmd cmd = { 0 };

                switch (parse_tokens(tokenbuffer, tokenbuffer_len, &cmd)) {
                    case __PARSER_SUCCESS__:
                        cmd_fprint(stdout, &cmd);

                        switch (exec_command(&cmd)) {
                            case __ENGINE_ERR__(CrtDupTab):
                                PERR("Command ExecErr: Table need to create already exist");
                                break;
                            case __ENGINE_ERR__(CmdType):
                                PERR("Command ExecErr: Unknown Command Type");
                                break;
                            case __ENGINE_ERR__(CrtCmdType):
                                PERR("Command ExecErr: Unknown Create Command Type");
                                break;
                            case __ENGINE_ERR__(ShowCmdType):
                                PERR("Command ExecErr: Unknown Show Command Hierachy");
                                break;
                            case __ENGINE_ERR__(ShowCmdAttrType):
                                PERR("Command ExecErr: Unknown Show Command Attr");
                                break;
                            default:
                                break;
                        }
                        break;
                    case __PARSER_ERR__(TabColNameLen):
                        PERR("Invalid Command: Columns' Name too long");
                        break;
                    default:
                        PERR("Invalid Command: Invalid Token Or Syntax");
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

                PERR("Invalid Command: Too few tokens");

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
