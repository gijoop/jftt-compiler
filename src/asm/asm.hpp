#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "types.hpp"
#include "util.hpp"

namespace Asm {

enum class Code {
    READ, WRITE,
    LOAD, STORE,
    RLOAD, RSTORE,
    ADD, SUB, SWAP, RESET, INC, DEC, SHL, SHR,
    JUMP, JPOS, JZERO, CALL, RET,
    HALT,
    LABEL // Specjalny "rozkaz" oznaczający miejsce etykiety
};

struct Label {
    Label(std::string name) : name(std::move(name)) {}
    Label() : name("") {}

    std::string name;
    long long address = -1;
};

struct Instruction {
    Code op;
    
    Register reg = Register::RA;
    long long value = 0;
    std::shared_ptr<Label> label = nullptr;

    std::string to_string(int level = 0) const {
        std::string instr_str;

        switch (op) {
            case Code::READ:   instr_str = "READ"; break;
            case Code::WRITE:  instr_str = "WRITE"; break;
            case Code::HALT:   instr_str = "HALT"; break;
            case Code::RET:    instr_str = "RTRN"; break;

            case Code::LOAD:   instr_str = "LOAD " + std::to_string(value); break;
            case Code::STORE:  instr_str = "STORE " + std::to_string(value); break;
            
            // Rejestrowe
            case Code::RLOAD:  instr_str = "RLOAD " + util::to_string(reg); break;
            case Code::RSTORE: instr_str = "RSTORE " + util::to_string(reg); break;
            case Code::ADD:    instr_str = "ADD " + util::to_string(reg); break;
            case Code::SUB:    instr_str = "SUB " + util::to_string(reg); break;
            case Code::SWAP:   instr_str = "SWP " + util::to_string(reg); break;
            case Code::RESET:  instr_str = "RST " + util::to_string(reg); break;
            case Code::INC:    instr_str = "INC " + util::to_string(reg); break;
            case Code::DEC:    instr_str = "DEC " + util::to_string(reg); break;
            case Code::SHL:    instr_str = "SHL " + util::to_string(reg); break;
            case Code::SHR:    instr_str = "SHR " + util::to_string(reg); break;

            // Skoki (obsługa przypadku, gdy label jest null, choć nie powinno się to zdarzyć)
            case Code::JUMP:   instr_str = "JUMP " + (label ? std::to_string(label->address) : "?"); break;
            case Code::JPOS:   instr_str = "JPOS " + (label ? std::to_string(label->address) : "?"); break;
            case Code::JZERO:  instr_str = "JZERO " + (label ? std::to_string(label->address) : "?"); break;
            case Code::CALL:   instr_str = "CALL " + (label ? std::to_string(label->address) : "?"); break;

            // Etykieta
            case Code::LABEL:  return "# LABEL " + (label ? label->name : "???");
        }

        return util::pad(level) + instr_str;
    }
};

inline Instruction make(Code op, Register reg) {
    Instruction i;
    i.op = op;
    i.reg = reg;
    return i;
}

inline Instruction make(Code op, long long val) {
    Instruction i;
    i.op = op;
    i.value = val;
    return i;
}

inline Instruction make(Code op, std::shared_ptr<Label> label) {
    Instruction i;
    i.op = op;
    i.label = std::move(label);
    return i;
}

inline Instruction make(Code op) {
    Instruction i;
    i.op = op;
    return i;
}

} // namespace Asm