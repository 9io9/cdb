#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#include "third_party/cstr/cstr.h"
#include "third_party/cprt/cprt.h"

#include "db_parser/parser.h"
#include "db_shell/shell.h"

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
