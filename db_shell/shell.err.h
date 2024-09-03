//
// Created by mario on 2024/9/2.
//

#ifndef SHELL_ERR_H
#define SHELL_ERR_H

#define __NAME__(n) CDBShell ## n

typedef enum CDBShellStatusCode {
    __NAME__(Success) = 0,
    __NAME__(InvalidCharError),

}CDBShellStatusCode;

#undef __NAME__
#endif //SHELL_ERR_H
