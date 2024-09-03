//
// Created by mario on 2024/9/2.
//

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "../third_party/cstr/cstr.h"

#include "shell.err.h"

#define __SHELL_ERR__(e) CDBShell ## e ## Error
#define __SHELL_SUCCESS__ CDBShellSuccess

static const char* SPEC_CHARSET = ",():";
static const char* SEP_CHARSET = "_@$#";

static bool is_spec(char ch) {
    int spec_id = 0;

    while (SPEC_CHARSET[spec_id] != '\0') {
        if (SPEC_CHARSET[spec_id++] == ch) {
            return true;
        }
    }

    return false;
}

static bool is_sep(char ch) {
    int sep_id = 0;

    while (SEP_CHARSET[sep_id] != '\0') {
        if (SEP_CHARSET[sep_id++] == ch) {
            return true;
        }
    }

    return false;
}

CDBShellStatusCode read_line(FILE* f, CString* linebuffer) {
    fprintf(stdout, "cdb>");

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
            if (!is_sep(input_ch)) {
                fprintf(stderr, "CDB Error: Invalid Character\n");
                fflush(stdin);
                fprintf(stdout, "cbd>");
                return __SHELL_ERR__(InvalidChar);
            }
            cstr_pushc_nocheck(input_ch, linebuffer);
        }
    }

    fflush(stdin);

    return __SHELL_SUCCESS__;
}
