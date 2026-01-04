#pragma once

#include <string>
#include <vector>
#include <memory>

#include "util.hpp"
#include "ast/asm.hpp"

namespace Tac {

using ASMCode = std::vector<std::unique_ptr<AsmAST::Node>>;

// Helper functions to create ASM instructions
inline void append_code(ASMCode& dest, ASMCode src) {
    dest.insert(
        dest.end(), 
        std::make_move_iterator(src.begin()), 
        std::make_move_iterator(src.end())
    );
}

template <typename T, typename... Args>
auto make_instr(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

inline auto asm_halt()                        { return make_instr<AsmAST::HaltNode>(); }
inline auto asm_store(long long address)      { return make_instr<AsmAST::StoreNode>(address); }
inline auto asm_load(long long address)       { return make_instr<AsmAST::LoadNode>(address); }
inline auto asm_write()                       { return make_instr<AsmAST::WriteNode>(); }
inline auto asm_swap(Register reg)            { return make_instr<AsmAST::SwapNode>(reg); }
inline auto asm_reset(Register reg)           { return make_instr<AsmAST::ResetNode>(reg); }
inline auto asm_add(Register reg)             { return make_instr<AsmAST::AddNode>(reg); }
inline auto asm_subtract(Register reg)        { return make_instr<AsmAST::SubtractNode>(reg); }
inline auto asm_shift_left(Register reg)      { return make_instr<AsmAST::ShiftLeftNode>(reg); }
inline auto asm_shift_right(Register reg)     { return make_instr<AsmAST::ShiftRightNode>(reg); }
inline auto asm_increment(Register reg)       { return make_instr<AsmAST::IncrementNode>(reg); }

class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_string(int level = 0) const = 0;
    virtual ASMCode to_asm() const = 0;
};

class InstructionNode : public Node {};
class ValueNode : public Node {};

class ConstantNode : public ValueNode {
public:
    ConstantNode(long long value) : value_(value) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "CONSTANT: " + std::to_string(value_);
    }

    ASMCode to_asm() const override {
        ASMCode code;
        code.push_back(asm_reset(Register::RA));
        
        if (value_ == 0) return code;

        std::vector<bool> bits;
        bits.reserve(64);
        long long temp = value_;
        while (temp > 0) {
            bits.push_back(temp % 2 != 0);
            temp /= 2;
        }

        for (int i = bits.size() - 1; i >= 0; --i) {
            if (bits[i]) {
                code.push_back(std::make_unique<AsmAST::IncrementNode>(Register::RA));
            }
            if (i > 0) {
                code.push_back(std::make_unique<AsmAST::ShiftLeftNode>(Register::RA));
            }
        }
        return code;
    }
private:
    long long value_;
};

class VarNode : public ValueNode {
public:
    VarNode(const std::string& name) : name_(name) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "VARIABLE: " + name_;
    }

    const std::string& get_name() const {
        return name_;
    }

    ASMCode to_asm() const override {
        ASMCode code;
        auto address = SymbolTable::get_address(name_);
        code.push_back(asm_load(address));
        return code;
    }
private:
    std::string name_;
};

class CopyNode : public InstructionNode {
public:
    CopyNode(std::unique_ptr<VarNode> target, std::unique_ptr<ValueNode> source) : 
        target_(std::move(target)), 
        source_(std::move(source)) {}
    
    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = " + source_->to_string(0);
    }
    ASMCode to_asm() const override {
        ASMCode code;
        auto target_addr = SymbolTable::get_address(target_->get_name());

        append_code(code, std::move(source_->to_asm()));

        code.push_back(asm_store(target_addr));

        return code;
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> source_;
};

class WriteNode : public InstructionNode {
public:
    WriteNode(std::unique_ptr<ValueNode> val) : val_(std::move(val)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "WRITE:\n" + val_->to_string(level + 1);
    }

    ASMCode to_asm() const override {
        ASMCode code = val_->to_asm();
        code.push_back(std::make_unique<AsmAST::WriteNode>());
        return code;
    }

private:
    std::unique_ptr<ValueNode> val_;
};

class ReadNode : public InstructionNode {
public:
    ReadNode(std::unique_ptr<VarNode> target) : target_(std::move(target)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "READ INTO:\n" + target_->to_string(level + 1);
    }

    ASMCode to_asm() const override {
        ASMCode code;
        auto target_addr = SymbolTable::get_address(target_->get_name());

        code.push_back(std::make_unique<AsmAST::ReadNode>());

        code.push_back(asm_store(target_addr));

        return code;
    }
private:
    std::unique_ptr<VarNode> target_;
};

class AddNode : public InstructionNode {
public:
    AddNode(std::unique_ptr<VarNode> target, 
            std::unique_ptr<ValueNode> left, 
            std::unique_ptr<ValueNode> right) : 
        target_(std::move(target)), 
        left_(std::move(left)), 
        right_(std::move(right)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = " 
               + left_->to_string(0) + " + " + right_->to_string(0);
    }

    ASMCode to_asm() const override {
        ASMCode code;
        auto target_addr = SymbolTable::get_address(target_->get_name());

        append_code(code, std::move(right_->to_asm()));

        code.push_back(asm_swap(Register::RB));

        append_code(code, std::move(left_->to_asm()));

        code.push_back(asm_add(Register::RB));

        code.push_back(asm_store(target_addr));

        return code;
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};

class SubtractNode : public InstructionNode {
public:
    SubtractNode(std::unique_ptr<VarNode> target, 
                 std::unique_ptr<ValueNode> left, 
                 std::unique_ptr<ValueNode> right) :
        target_(std::move(target)), 
        left_(std::move(left)), 
        right_(std::move(right)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = " 
               + left_->to_string(0) + " - " + right_->to_string(0);
    }

    ASMCode to_asm() const override {
        ASMCode code;
        auto target_addr = SymbolTable::get_address(target_->get_name());

        append_code(code, std::move(right_->to_asm()));

        code.push_back(asm_swap(Register::RB));

        append_code(code, std::move(left_->to_asm()));

        code.push_back(asm_subtract(Register::RB));

        code.push_back(asm_store(target_addr));
        return code;
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};

class MultiplyNode : public InstructionNode {
public:
    MultiplyNode(std::unique_ptr<VarNode> target, 
                 std::unique_ptr<ValueNode> left, 
                 std::unique_ptr<ValueNode> right) :
        target_(std::move(target)), 
        left_(std::move(left)), 
        right_(std::move(right)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = " 
               + left_->to_string(0) + " * " + right_->to_string(0);
    }

    ASMCode to_asm() const override {
        return ASMCode{}; // Need to wait for IF implementation
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};

class DivideNode : public InstructionNode {
public:
    DivideNode(std::unique_ptr<VarNode> target, 
               std::unique_ptr<ValueNode> left, 
               std::unique_ptr<ValueNode> right) :
        target_(std::move(target)), 
        left_(std::move(left)), 
        right_(std::move(right)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = " 
               + left_->to_string(0) + " / " + right_->to_string(0);
    }

    ASMCode to_asm() const override {
        return ASMCode{}; // Need to wait for IF implementation
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};

class ModuloNode : public InstructionNode {
public:
    ModuloNode(std::unique_ptr<VarNode> target, 
               std::unique_ptr<ValueNode> left, 
               std::unique_ptr<ValueNode> right) :
        target_(std::move(target)), 
        left_(std::move(left)), 
        right_(std::move(right)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = " 
               + left_->to_string(0) + " % " + right_->to_string(0);
    }

    ASMCode to_asm() const override {
        return ASMCode{}; // Need to wait for IF implementation
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};

class ProgramNode : public Node {
public:
    ProgramNode(std::vector<std::unique_ptr<InstructionNode>> instructions) : 
        instructions_(std::move(instructions)) {}

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "PROGRAM:\n";
        for (const auto& instr : instructions_) {
            result += instr->to_string(level + 1) + "\n";
        }
        return result;
    }

    ASMCode to_asm() const override {
        ASMCode code;
        for (const auto& instr : instructions_) {
            append_code(code, instr->to_asm());
        }
        code.push_back(std::make_unique<AsmAST::HaltNode>());
        return code;
    }
private:
    std::vector<std::unique_ptr<InstructionNode>> instructions_;
};

} // namespace Tac