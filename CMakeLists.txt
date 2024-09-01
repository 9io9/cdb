cmake_minimum_required(VERSION 3.28)
project(cdb C)

set(CMAKE_C_STANDARD 17)

add_subdirectory(cstr)

add_subdirectory(cvla)

add_executable(cdb cdb.c
        cdb.def.h
        cdb.err.h)

target_link_libraries(cdb cstr cvla)

add_compile_definitions(__MACHINE_WORD_SIZE__=64)