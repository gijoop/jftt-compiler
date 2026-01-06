#pragma once

#include "ast/ast.hpp"
#include <string>
#include <memory>

class ParserWrapper {
public:
    ParserWrapper();
    ~ParserWrapper();

    bool parse(const std::string& input);
    std::unique_ptr<AST::ProgramNode> get_result() { return std::move(result); }

private:
    std::unique_ptr<AST::ProgramNode> result;
    void* scanner;
};