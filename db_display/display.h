//
// Created by mario on 2024/9/3.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include "../third_party/cstr/cstr.def.h"
#include "../db_engine/table.def.h"
#include "../db_parser/parser.def.h"
#include "../db_apis/name.h"
#include "display.err.h"

extern CDBDisplayStatusCode kv_ref_fprint(FILE* f, const char* key, CStringRef* value);

extern CDBDisplayStatusCode kv_str_fprint(FILE* f, const char* key, const char* value);

extern CDBDisplayStatusCode title_fprint(FILE* f, const char* title, const char tsep);

extern CDBDisplayStatusCode end_fprint(FILE* f, const char tsep);

extern CDBDisplayStatusCode kv_tabcols_fprint(FILE* f, const char* key, CDBTableColumn* cols, size_t cols_size);

extern CDBDisplayStatusCode kv_tabcol_fprint(FILE* f, CDBTableColumn* col);

extern CDBDisplayStatusCode cmd_fprint(FILE* f, CDBCmd* cmd);

// no func arg check version

FUNC_ASSERT(F != NULL, KEY != NULL, VALUE != NULL)
extern CDBDisplayStatusCode kv_ref_fprint_nocheck(FILE* f, const char* key, CStringRef* value);

FUNC_ASSERT(F != NULL, KEY != NULL, VALUE != NULL)
extern CDBDisplayStatusCode kv_str_fprint_nocheck(FILE* f, const char* key, const char* value);

FUNC_ASSERT(F != NULL, TITLE != NULL)
extern CDBDisplayStatusCode title_fprint_nocheck(FILE* f, const char* title, const char tsep);

FUNC_ASSERT(F != NULL)
extern CDBDisplayStatusCode end_fprint_nocheck(FILE* f, const char tsep);

FUNC_ASSERT(F != NULL, KEY != NULL, COLS != NULL, COL_SIZE != 0)
extern CDBDisplayStatusCode kv_tabcols_fprint_nocheck(FILE* f, const char* key, CDBTableColumn* cols, size_t cols_size);

FUNC_ASSERT(F != NULL, COL != NULL)
extern CDBDisplayStatusCode kv_tabcol_fprint_nocheck(FILE* f, CDBTableColumn* col);

FUNC_ASSERT(F != NULL, CMD != NULL)
extern CDBDisplayStatusCode cmd_fprint_nocheck(FILE* f, CDBCmd* cmd);

#endif //DISPLAY_H
