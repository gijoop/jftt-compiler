cmake_minimum_required(VERSION 3.16)
project(JfttCompiler LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(Parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)

FLEX_TARGET(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_executable(compiler
    src/main.cpp
    src/parser_wrapper.cpp
    src/analyzer/ast_visitor.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

target_compile_options(compiler PRIVATE -Wno-free-nonheap-object)

target_include_directories(compiler PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    src
)
