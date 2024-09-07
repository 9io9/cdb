//
// Created by mario on 2024/9/7.
//

#ifndef ACCESS_FUNC_H
#define ACCESS_FUNC_H

#include "../db_parser/parser.def.h"
#include "../db_engine/table.def.h"

// access functions for those member which need to use above three levels access op
static inline char* crtcmd_tabname(CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name->_cstr_ref;
}

static inline void crtcmd_set_tabname(CDBCmd* cmd, CStringRef* tabname) {
    cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name = tabname;
}

static inline size_t crtcmd_tabname_len(CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_name->_cstr_ref_len;
}

static inline size_t crtcmd_tabcols_size(CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz;
}

static inline size_t* crtcmd_tabcols_size_ref(CDBCmd* cmd) {
    return &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla_sz;
}

static inline CDBTableColumn* crtcmd_tabcols(CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols->_cvla;
}

static inline CVLArray* crtcmd_tabcols_cvla(CDBCmd* cmd) {
    return cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols;
}

static inline CVLArray** crtcmd_tabcols_cvla_ref(CDBCmd* cmd) {
    return &cmd->_cdb_cmd._crt_cmd._cmd._tab._tab_cols;
}

static inline char* showcmd_name(CDBCmd* cmd) {
    return cmd->_cdb_cmd._show_cmd._name->_cstr_ref;
}

static inline size_t showcmd_name_len(CDBCmd* cmd) {
    return cmd->_cdb_cmd._show_cmd._name->_cstr_ref_len;
}


#endif //ACCESS_FUNC_H
