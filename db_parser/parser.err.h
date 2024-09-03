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
    // invalid seperators in a command (e.g. create table _t (,name:CharArr);[no column occurs before ','])
    __NAME__(SepFmtError),
    // table column's format is invalid (e.g. create table _t (name);[no ':' after col's name and col's type missing])
    __NAME__(TabColFmtError),
    // unknown type is specified for a column in a command (e.g. create table _t (name:UNKNOWN_TYPE);)
    __NAME__(TabColTypeError),
    // wrong command format is given (e.g. create table _t (name:int) OTHER_WORDS...; or INVALID_WORDS...;)
    __NAME__(CmdFmtError),
    // invalid function arguments given when calling parser's funcs
    __NAME__(FuncArgError),
    // invalid command type given when calling parser's funcs
    __NAME__(CmdTypeError),
    // invalid create command type given when calling parser's funcs
    __NAME__(CrtCmdTypeError),
}CDBParserStatusCode;

#undef __NAME__

#endif //PARSER_ERR_H
