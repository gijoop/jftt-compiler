#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace Tac {

enum class OperandType { CONSTANT, VARIABLE, TEMP, LABEL, REFERENCE };

struct Operand {
    OperandType type;
    std::string name; // Dla VARIABLE i LABEL
    long long value;  // Dla CONSTANT
    int temp_id;      // Dla TEMP (np. 0 dla t0, 1 dla t1)

    std::string to_string() const {
        switch(type) {
            case OperandType::CONSTANT: return std::to_string(value);
            case OperandType::VARIABLE: return name;
            case OperandType::TEMP: return "tmp." + std::to_string(temp_id);
            case OperandType::LABEL: return name;
            case OperandType::REFERENCE: return "&" + name;
        }
        return "?";
    }

    // Helpers
    static Operand make_const(long long val) { return {OperandType::CONSTANT, "", val, 0}; }
    static Operand make_var(const std::string& n) { return {OperandType::VARIABLE, n, 0, 0}; }
    static Operand make_temp(int id) { return {OperandType::TEMP, "", 0, id}; }
    static Operand make_label(const std::string& n) { return {OperandType::LABEL, n, 0, 0}; }
    static Operand make_reference(const std::string& n) { return {OperandType::REFERENCE, n, 0, 0}; }
};

enum class OpCode {
    NOP,
    ASSIGN,     // x = y
    ADD, SUB, MUL, DIV, MOD,
    EQ, NEQ, LT, GT, LTE, GTE, // Wynik logiczny do zmiennej
    JMP,        // Skok bezwarunkowy
    JZERO,       // Skok jeśli arg1 == 0 (False)
    JPOS,        // Skok jeśli arg1 > 0
    LABEL,      // Miejsce docelowe skoku (arg1 to nazwa etykiety)
    CALL,       // Wywołanie procedury
    RETURN,    // Powrót z procedury
    FUNC_ENTER,
    FUNC_EXIT,
    PARAM,
    WRITE,
    READ,
    HALT
};

struct Instruction {
    OpCode op;
    std::optional<Operand> arg1;
    std::optional<Operand> arg2;
    std::optional<Operand> result;

    std::string to_string() const {
        std::string s = "";
        if (result) s += result->to_string() + " = ";
        
        switch(op) {
            case OpCode::ADD: s += "ADD "; break;
            case OpCode::SUB: s += "SUB "; break;
            case OpCode::MUL: s += "MUL "; break;
            case OpCode::DIV: s += "DIV "; break;
            case OpCode::MOD: s += "MOD "; break;
            case OpCode::EQ: s += "EQ "; break;
            case OpCode::NEQ: s += "NEQ "; break;
            case OpCode::LT: s += "LT "; break;
            case OpCode::GT: s += "GT "; break;
            case OpCode::LTE: s += "LTE "; break;
            case OpCode::GTE: s += "GTE "; break;
            case OpCode::LABEL: return arg1->to_string() + ":";
            case OpCode::JMP: return "JMP " + arg1->to_string();
            case OpCode::JZERO: return "JZERO " + arg1->to_string() + " GOTO " + result->to_string();
            case OpCode::JPOS: return "JPOS " + arg1->to_string() + " GOTO " + result->to_string();
            case OpCode::CALL: s += "CALL "; break;
            case OpCode::RETURN: s += "RETURN "; break;
            case OpCode::FUNC_ENTER: s += "FUNC_ENTER "; break;
            case OpCode::FUNC_EXIT: s += "FUNC_EXIT "; break;
            case OpCode::PARAM: s += "PARAM "; break;
            case OpCode::ASSIGN: s += "ASSIGN "; break;
            case OpCode::WRITE: s += "WRITE "; break;
            case OpCode::READ: s += "READ "; break;
            case OpCode::HALT: s += "HALT"; break;
            default: s += "OP "; break;
        }

        if (arg1) s += arg1->to_string();
        if (arg2) s += ", " + arg2->to_string();
        return s;
    }
};

class Program {
public:
    std::vector<Instruction> instructions;

    Operand new_temp() {
        return Operand::make_temp(temp_counter++);
    }

    Operand new_label() {
        return Operand::make_label("L" + std::to_string(label_counter++));
    }

    void emit(Instruction instr) {
        instructions.push_back(instr);
    }

    int next_temp_id() const {
        return temp_counter;
    }

private:
    int temp_counter = 0;
    int label_counter = 0;
};

} // namespace Tac