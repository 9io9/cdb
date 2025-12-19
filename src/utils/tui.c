#include <stdio.h>

#include "utils/tui.h"

errinfo get_line(String *line) {
    i8 c;
    errinfo e;

    while ((c = getchar()) != ';') {
        e = string_push_i8(line, c);

        if (ECHECK(e)) {
            return e;
        }
    }

    return EINFO(0, NULL);
}