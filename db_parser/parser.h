//
// Created by mario on 2024/9/1.
//

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "parser.def.h"
#include "parser.err.h"

#include "../db_engine/table.def.h"

extern CDBParserStatusCode parse_col_type(CStringRef* token, CDBTableColumnType* col_type);

extern CDBParserStatusCode parse_tab_cols(CStringRef* tokens, size_t tokens_size, size_t* token_id, CVLArray** cols);

extern CDBParserStatusCode parse_tokens(CStringRef* tokens, size_t tokens_size, CDBCmd* cmd);

extern CDBParserStatusCode cmd_fprint(FILE* f, CDBCmd* cmd);

// no func arg check version

FUNC_ASSERT(TOKEN != NULL, COL_TYPE != NULL)
extern CDBParserStatusCode parse_col_type_nocheck(CStringRef* token, CDBTableColumnType* col_type);

FUNC_ASSERT(TOKENS != NULL, TOKENS_SIZE != 0, TOKEN_ID != NULL, COLS != NULL)
extern CDBParserStatusCode parse_tab_cols_nocheck(CStringRef* tokens, size_t tokens_size, size_t* token_id, CVLArray** cols);

FUNC_ASSERT(TOKENS != NULL, TOKENS_SIZE != 0, CMD != NULL)
extern CDBParserStatusCode parse_tokens_nocheck(CStringRef* tokens, size_t tokens_size, CDBCmd* cmd);

FUNC_ASSERT(F != NULL, CMD != NULL)
extern CDBParserStatusCode cmd_fprint_nocheck(FILE* f, CDBCmd* cmd);

#endif //PARSER_H
