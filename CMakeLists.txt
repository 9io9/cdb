cmake_minimum_required(VERSION 3.28)
project(cdb C)

set(CMAKE_C_STANDARD 17)

execute_process(COMMAND git clone git@github.com:9io9/cstr.git WORKING_DIRECTORY third_party)

add_subdirectory(third_party/cstr)

add_subdirectory(third_party/cvla)

add_subdirectory(third_party/cprt)

add_executable(cdb cdb.c
        cdb.err.h
        db_parser/parser.c
        db_parser/parser.h
        db_parser/parser.err.h
        db_parser/parser.def.h
        db_engine/table.def.h
        db_shell/shell.c
        db_shell/shell.h
        db_shell/shell.err.h
        db_engine/engine.c
        db_engine/engine.h
        db_engine/engine.err.h
        db_display/display.h
        db_display/display.c
        db_display/display.err.h
        db_apis/name.h
        db_apis/access_func.h
        db_engine/database.def.h
        db_engine/engine.def.h
        db_buffer/buffer.h
        db_buffer/buffer.def.h
        db_buffer/buffer.err.h
        db_apis/consts.h
        db_buffer/buffer.c
)

target_link_libraries(cdb cstr cvla cprt)

add_compile_definitions(__MACHINE_WORD_SIZE__=64)