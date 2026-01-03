#pragma once

#include "ast/lang.hpp"
#include <string>
#include <memory>

class ParserWrapper {
public:
    ParserWrapper();
    ~ParserWrapper();

    bool parse(const std::string& input);
    std::unique_ptr<LangAST::Node> get_result() { return std::move(result); }

private:
    std::unique_ptr<LangAST::Node> result;
    void* scanner;
};