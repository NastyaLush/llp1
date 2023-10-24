cmake_minimum_required(VERSION 3.0.0)
project(LLPLAB VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(LLPLAB main.c
        code/io.h
        code/table_operations.c
        code/graph_stracture.h
        code/table_operations.h
        code/io.c
        code/graph_stracture.c
        code/graph_stracture.c
        code/mmap_functions.h
        code/mmap_functions.c
        code/node_operations.c
        code/page_operations.h
        code/page_operations.c
        code/node_operations.h
        code/iterator.h
        code/iterator.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
