//
// Created by mario on 2024/9/4.
//

#ifndef NAME_H
#define NAME_H

#define __CMD__(c) CDBCmd ## c
#define __HIER__(h)  CDBHier ## h
#define __ATTR__(a) CDBAttr ## a

#define __PARSER_ERR__(e) CDBParser ## e ## Error
#define __PARSER_SUCCESS__ CDBParserSuccess

#define __DISP_ERR__(e) CDBDisplay ## e ## Error
#define __DISP_SUCCESS__ CDBDisplaySuccess

#define __ENGINE_ERR__(e) CDBEngine ## e ## Error
#define __ENGINE_SUCCESS__ CDBEngineSuccess

#define __SHELL_ERR__(e) CDBShell ## e ## Error
#define __SHELL_SUCCESS__ CDBShellSuccess

#define FUNC_ASSERT(...)

#endif //NAME_H
