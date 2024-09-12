//
// Created by mario on 2024/9/7.
//

#ifndef ENGINE_DEF_H
#define ENGINE_DEF_H

#include "../db_apis/consts.h"

typedef enum CDBFileType {
    CDBTableFile = 0,
    CDBTabIndexFile,
    CDBColFile,
}CDBFileType;

#endif //ENGINE_DEF_H
