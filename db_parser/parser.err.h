//
// Created by mario on 2024/9/1.
//

#ifndef PARSER_ERR_H
#define PARSER_ERR_H

#define __NAME__(n) CDBParser ## n

typedef enum CDBParserStatusCode {
    // no invalid format in commands
    __NAME__(Success) = 0,
    // table format specified in create command is invalid (e.g. create table _t;[no columns specified])
    __NAME__(CrtTabFmtError),
    // unknown create type specified in a create command
    __NAME__(CrtTypeError),
    // invalid seperators in a command (e.g. create table _t (,name:CharArr);[no column occurs before ','])
    __NAME__(SepFmtError),
    // table column's format is invalid (e.g. create table _t (name);[no ':' after col's name and col's type missing])
    __NAME__(TabColFmtError),
    // unknown type is specified for a column in a command (e.g. create table _t (name:UNKNOWN_TYPE);)
    __NAME__(TabColTypeError),
    // name of table column too long to store into disk
    __NAME__(TabColNameLenError),
    // wrong command format is given (e.g. create table _t (name:int) OTHER_WORDS...; or INVALID_WORDS...;)
    __NAME__(CmdFmtError),
    // invalid function arguments given when calling parser's funcs
    __NAME__(FuncArgError),
    // unknown attr type is specified for a show command
    __NAME__(ShowCmdAttrTypeError),
    // row format specified in insert command is invalid (e.g. insert row tab_t; [no row content specified])
    __NAME__(InsRowFmtError),
    // unknown insert type specified in an insert command
    __NAME__(InsTypeError),
    // invalid values occur in a inserting row (e.g. insert row _t (p:int)[p is not an integer])
    __NAME__(InsRowValueError),
}CDBParserStatusCode;

#undef __NAME__

#endif //PARSER_ERR_H
