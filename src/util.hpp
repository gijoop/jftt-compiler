#pragma once

#include <string>

#include "register.hpp"
#include "operator.hpp"

namespace util {

inline std::string pad(int level) {
    return std::string(level * 2, ' ');
}

inline std::string to_string(Register reg) {
    switch (reg) {
        case Register::RA:
            return "a";
        case Register::RB:
            return "b";
        case Register::RC:
            return "c";
        case Register::RD:
            return "d";
        case Register::RE:
            return "e";
        case Register::RF:
            return "f";
        case Register::RG:
            return "g";
        case Register::RH:
            return "h";
        default:
            return "unknown";
    }
}

inline std::string to_string(BinaryOp op) {
    switch (op) {
        case BinaryOp::ADD:
            return "+";
        case BinaryOp::SUB:
            return "-";
        case BinaryOp::MUL:
            return "*";
        case BinaryOp::DIV:
            return "/";
        case BinaryOp::MOD:
            return "%";
        default:
            return "unknown";
    }
}

} // namespace util