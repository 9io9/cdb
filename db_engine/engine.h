//
// Created by mario on 2024/9/3.
//

#ifndef ENGINE_H
#define ENGINE_H

#include "../db_parser/parser.def.h"

#include "engine.err.h"

extern CDBEngineStatusCode exec_command(CDBCmd* cmd);

#endif //ENGINE_H
