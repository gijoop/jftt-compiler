#pragma once

#include <string>

#include "types.hpp"
#include "tac/tac.hpp"

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

inline std::string to_string(BinaryCondOp op) {
    switch (op) {
        case BinaryCondOp::EQ:
            return "==";
        case BinaryCondOp::NEQ:
            return "!=";
        case BinaryCondOp::LT:
            return "<";
        case BinaryCondOp::LTE:
            return "<=";
        case BinaryCondOp::GT:
            return ">";
        case BinaryCondOp::GTE:
            return ">=";
        default:
            return "unknown";
    }
}

inline Tac::OpCode to_tac_op(BinaryOp op) {
    switch (op) {
        case BinaryOp::ADD:
            return Tac::OpCode::ADD;
        case BinaryOp::SUB:
            return Tac::OpCode::SUB;
        case BinaryOp::MUL:
            return Tac::OpCode::MUL;
        case BinaryOp::DIV:
            return Tac::OpCode::DIV;
        case BinaryOp::MOD:
            return Tac::OpCode::MOD;
        default:
            throw std::runtime_error("Unknown BinaryOp");
    }
}

inline Tac::OpCode to_tac_op(BinaryCondOp op) {
    switch (op) {
        case BinaryCondOp::EQ:
            return Tac::OpCode::EQ;
        case BinaryCondOp::NEQ:
            return Tac::OpCode::NEQ;
        case BinaryCondOp::LT:
            return Tac::OpCode::LT;
        case BinaryCondOp::LTE:
            return Tac::OpCode::LTE;
        case BinaryCondOp::GT:
            return Tac::OpCode::GT;
        case BinaryCondOp::GTE:
            return Tac::OpCode::GTE;
        default:
            throw std::runtime_error("Unknown BinaryCondOp");
    }
}

inline std::string quote(const std::string& str) {
    return "\"" + str + "\"";
}

} // namespace util