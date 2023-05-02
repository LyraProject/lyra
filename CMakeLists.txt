cmake_minimum_required(VERSION 3.16)

project(lyra)

option(LYRA_BUILD_EXAMPLES "Build lyra usage examples from the examples folder" OFF)
option(LYRA_BUILD_TESTS "Build lyra unit tests" OFF)

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

# tests are c++
set(LYRA_TEST_SRCS
    src/tests/lyra_cbuf_tests.cpp
    )

add_library(lyra SHARED ${LYRA_SRCS})
target_include_directories(lyra PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(lyra_s STATIC ${LYRA_SRCS})
target_include_directories(lyra_s PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

set_project_warnings(lyra)

if (${LYRA_BUILD_EXAMPLES})
    add_executable(example1 examples/example1.c)
    target_link_libraries(example1 lyra_s)
    add_executable(example2 examples/example2.c)
    target_link_libraries(example2 lyra_s)
    add_executable(example3 examples/example3.c)
    target_link_libraries(example3 lyra_s)
endif()

if (${LYRA_BUILD_TESTS})
    # get googletest
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # for windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # make available the target
    FetchContent_MakeAvailable(googletest)

    add_executable(lyra_tests ${LYRA_TEST_SRCS})
    target_link_libraries(lyra_tests GTest::gtest_main lyra_s)

    include(GoogleTest)
    gtest_discover_tests(lyra_tests)
endif()

