#pragma once

#include <stdexcept>
#include <string>

class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string& message) :
        std::runtime_error("Semantic Error: " + message) {}
};