#pragma once

#include "ast/lang.hpp"
#include <string>
#include <memory>

class ParserWrapper {
public:
    ParserWrapper();
    ~ParserWrapper();

    bool parse(const std::string& input);
    std::unique_ptr<LangAST::ProgramNode> get_result() { return std::move(result); }

private:
    std::unique_ptr<LangAST::ProgramNode> result;
    void* scanner;
};