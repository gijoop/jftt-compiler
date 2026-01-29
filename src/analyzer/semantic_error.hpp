#pragma once

#include <stdexcept>
#include <string>

class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string& message, int line = -1) :
        std::runtime_error(line > 0 ? 
            "Semantic Error at line " + std::to_string(line) + ": " + message :
            "Semantic Error: " + message) {}
};