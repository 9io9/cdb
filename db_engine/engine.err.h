//
// Created by mario on 2024/9/3.
//

#ifndef ENGINE_ERR_H
#define ENGINE_ERR_H

#define __NAME__(n) CDBEngine ## n

typedef enum CDBEngineStatusCode {
    __NAME__(Success) = 0,
    // invalid arguments are given when calling engine's funcs
    __NAME__(FuncArgError),
    // invalid command type when command is executed by engine
    __NAME__(CmdTypeError),
    // invalid create command type when command is executed by engine
    __NAME__(CrtCmdTypeError),
    // table in create command has already been created
    __NAME__(CrtDupTabError),
    // table's name too long to create
    __NAME__(CrtTabNameTooLongError),
    // name of table to be shown not exist
    __NAME__(ShowTabNotExistError),
    // invalid show command type when command is executed by engine
    __NAME__(ShowCmdTypeError),
    // invalid show command attr type when command is executed by engine
    __NAME__(ShowCmdAttrTypeError),
}CDBEngineStatusCode;

#undef __NAME__

#endif //ENGINE_ERR_H
