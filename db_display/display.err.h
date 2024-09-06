//
// Created by mario on 2024/9/3.
//

#ifndef DISPLAY_ERR_H
#define DISPLAY_ERR_H

#define __NAME__(n) CDBDisplay ## n

typedef enum CDBDisplayStatusCode {
    __NAME__(Success) = 0,
    __NAME__(FuncArgError),
    __NAME__(CmdTypeError)
}CDBDisplayStatusCode;

#undef __NAME__
#endif //DISPLAY_ERR_H
