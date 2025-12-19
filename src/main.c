#include <stdio.h>
#include <string.h>

#include "utils/string.h"
#include "utils/tui.h"
#include "utils/vector.h"

typedef enum {
    Chars,
    Integer,
    Json,
    Binary,
    Array
}ColumnType;

typedef struct {
    ColumnType column_type;
    String column_name;
}ColumnDecl;

typedef struct {
    String table_name;
    Vector columns; // vector of ColumnDecl
}CrtTabSql;

typedef enum {
    CreateTable
}SqlType;

typedef struct {
    SqlType sql_type;
    union {
        CrtTabSql create_table_sql;
    }sql;
}Sql;

errinfo parse_sql(String* line, Sql* sql) {
    // TODO: parse line and form Sql structure for exectution
}

int main(int argc, char *argv[]) {
    String line;

    errinfo e = new_string(&line);

    if (ECHECK(e)) {
        EPRINT(e);
        return e.ecode;
    }

    setbuf(stdout, NULL);

    while (true) {
        printf("cdb> ");

        e = get_line(&line);

        if (ECHECK(e)) {
            EPRINT(e);
            return e.ecode;
        }

        printf("string read(%u): %s\n", line.size, line.string);

        u32 trim_begin, trim_end;

        strtrim(line.string, &trim_begin, &trim_end);

        line.string[trim_end + 1] = '\0';

        if (strcmp(&line.string[trim_begin], "exit") == 0) {
            break;
        } else {
            // parse command
        }

        line.size = 0;
    }

    return 0;
}