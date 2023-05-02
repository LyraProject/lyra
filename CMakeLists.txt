cmake_minimum_required(VERSION 3.16)

project(lyra)

option(LYRA_BUILD_EXAMPLES "Build lyra usage examples from the examples folder" OFF)

include(CompilerWarnings.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(LYRA_SRCS 
    src/lyra_slab.c
    src/lyra_cbuf.c
    src/lyra_tsque.c
    src/lyra_slab.h
    src/lyra_cbuf.h
    src/lyra_tsque.h
    src/lyra.h)

add_library(lyra SHARED ${LYRA_SRCS})
target_include_directories(lyra PUBLIC src)

add_library(lyra_s STATIC ${LYRA_SRCS})
target_include_directories(lyra_s PUBLIC src)

set_project_warnings(lyra)

if (${LYRA_BUILD_EXAMPLES})
    add_executable(example1 examples/example1.c)
    target_link_libraries(example1 lyra_s)
    add_executable(example2 examples/example2.c)
    target_link_libraries(example2 lyra_s)
    add_executable(example3 examples/example3.c)
    target_link_libraries(example3 lyra_s)
endif()

