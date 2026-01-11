cmake_minimum_required(VERSION 3.16)
project(JfttCompiler LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

BISON_TARGET(Parser src/parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)

FLEX_TARGET(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_library(compiler_core
    src/parser/parser_wrapper.cpp
    src/ast/visitor.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

target_include_directories(compiler_core PUBLIC src ${CMAKE_CURRENT_BINARY_DIR})

add_executable(compiler src/main.cpp)
target_link_libraries(compiler PRIVATE compiler_core)

add_executable(tests 
    test/write_test.cpp 
    test/var_test.cpp 
    test/decl_checker_test.cpp
    test/proc_checker_test.cpp
    test/add_test.cpp
    test/sub_test.cpp
    test/mul_test.cpp
    test/div_test.cpp
    test/mod_test.cpp
    test/if_test.cpp
    test/while_test.cpp
    test/repeat_test.cpp
    test/procedure_test.cpp
)
target_link_libraries(tests PRIVATE compiler_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
