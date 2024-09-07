//
// Created by mario on 2024/9/7.
//

#ifndef BUFFER_ERR_H
#define BUFFER_ERR_H

#define __NAME__(n) CDBBuffer ## n

typedef enum CDBBufferStatusCode {
    __NAME__(Success) = 0,
    __NAME__(FuncArgError),
#ifdef __MEM_SENSITIVE__
    __NAME__(NoMemError)
#endif
    __NAME__(FileReadError),
    __NAME__(FileWriteError),
    __NAME__(OverflowError),
}CDBBufferStatusCode;

#undef __NAME__
#endif //BUFFER_ERR_H
