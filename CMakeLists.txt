cmake_minimum_required(VERSION 3.16)

project(lyra)

option(LYRA_BUILD_EXAMPLES "Build lyra usage examples from the examples folder" OFF)

include(CompilerWarnings.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(lyra SHARED
    src/lyra.c
    src/lyra.h)
target_include_directories(lyra PUBLIC src)

add_library(lyra_s STATIC
    src/lyra.c
    src/lyra.h)
target_include_directories(lyra_s PUBLIC src)

set_project_warnings(lyra)

if (${LYRA_BUILD_EXAMPLES})
    add_executable(example1 examples/example1.c)
    target_link_libraries(example1 lyra_s)
endif()

