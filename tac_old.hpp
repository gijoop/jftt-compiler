#pragma once

#include <string>
#include <vector>
#include <memory>

#include "util.hpp"
#include "ast/asm.hpp"

namespace Tac {

using ASMCode = std::vector<std::unique_ptr<Asm::Node>>;

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

inline auto asm_halt()                        { SymbolTable::inc_ic(); return make_instr<Asm::HaltNode>(); }
inline auto asm_store(long long address)      { SymbolTable::inc_ic(); return make_instr<Asm::StoreNode>(address); }
inline auto asm_load(long long address)       { SymbolTable::inc_ic(); return make_instr<Asm::LoadNode>(address); }
inline auto asm_write()                       { SymbolTable::inc_ic(); return make_instr<Asm::WriteNode>(); }
inline auto asm_read()                        { SymbolTable::inc_ic(); return make_instr<Asm::ReadNode>(); }
inline auto asm_swap(Register reg)            { SymbolTable::inc_ic(); return make_instr<Asm::SwapNode>(reg); }
inline auto asm_reset(Register reg)           { SymbolTable::inc_ic(); return make_instr<Asm::ResetNode>(reg); }
inline auto asm_add(Register reg)             { SymbolTable::inc_ic(); return make_instr<Asm::AddNode>(reg); }
inline auto asm_subtract(Register reg)        { SymbolTable::inc_ic(); return make_instr<Asm::SubtractNode>(reg); }
inline auto asm_shift_left(Register reg)      { SymbolTable::inc_ic(); return make_instr<Asm::ShiftLeftNode>(reg); }
inline auto asm_shift_right(Register reg)     { SymbolTable::inc_ic(); return make_instr<Asm::ShiftRightNode>(reg); }
inline auto asm_increment(Register reg)       { SymbolTable::inc_ic(); return make_instr<Asm::IncrementNode>(reg); }
inline auto asm_jump(std::shared_ptr<Asm::Label> label) { SymbolTable::inc_ic(); return make_instr<Asm::JumpNode>(label); }
inline auto asm_jump_if_zero(std::shared_ptr<Asm::Label> label) { SymbolTable::inc_ic(); return make_instr<Asm::JumpIfZeroNode>(label); }
inline auto asm_jump_if_positive(std::shared_ptr<Asm::Label> label) { SymbolTable::inc_ic(); return make_instr<Asm::JumpIfPositiveNode>(label); }
inline auto asm_label_marker(std::shared_ptr<Asm::Label> label) { label->address_ = SymbolTable::get_ic(); return make_instr<Asm::LabelMarkerNode>(label); }

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
                code.push_back(asm_increment(Register::RA));
            }
            if (i > 0) {
                code.push_back(asm_shift_left(Register::RA));
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

    const std::string get_name() const {
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
        code.push_back(asm_write());
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

        code.push_back(asm_read());

        code.push_back(asm_store(target_addr));

        return code;
    }
private:
    std::unique_ptr<VarNode> target_;
};

/**
 * RESULT IN REGISTER A
 */
class CompareNode : public InstructionNode {
public:
    CompareNode(std::unique_ptr<VarNode> target,
                std::unique_ptr<ValueNode> left, 
                std::unique_ptr<ValueNode> right,
                BinaryCondOp op) :
        target_(std::move(target)), 
        left_(std::move(left)), 
        right_(std::move(right)),
        op_(op) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + target_->to_string(0) + " = COMPARE(" 
               + left_->to_string(0) + ", " + right_->to_string(0) 
               + ") WITH OP " + util::to_string(op_);
    }

    ASMCode to_asm() const override {
        ASMCode code;
        
        append_code(code, right_->to_asm());   // ra = right
        code.push_back(asm_swap(Register::RB)); // rb = right
        append_code(code, left_->to_asm());    // ra = left

        switch (op_) {
            case BinaryCondOp::GT: // ra > 0 -> true
                code.push_back(asm_subtract(Register::RB)); // ra = left - right
                break;
            case BinaryCondOp::LT: // ra > 0 -> true
                code.push_back(asm_swap(Register::RB));
                code.push_back(asm_subtract(Register::RB)); // ra = left - right
                break;
            case BinaryCondOp::EQ:
            case BinaryCondOp::NEQ: { // ra == 0 -> true
                code.push_back(asm_reset(Register::RC));
                code.push_back(asm_swap(Register::RC));
                code.push_back(asm_add(Register::RC)); // copy left to rc
                code.push_back(asm_subtract(Register::RB)); // ra = left - right
                code.push_back(asm_swap(Register::RB)); // rb = left - right
                code.push_back(asm_subtract(Register::RC)); // ra = right - left
                code.push_back(asm_add(Register::RB)); // ra = (left - right) + (right - left)
                break;
            }
            case BinaryCondOp::LTE: { // ra > 0 -> false
                code.push_back(asm_swap(Register::RB));
                code.push_back(asm_subtract(Register::RB)); // ra = right - left
                break;
            }
            case BinaryCondOp::GTE: { // ra > 0 -> false
                code.push_back(asm_subtract(Register::RB)); // ra = left - right
                break;
            }
        }

        return code;
    }
private:
    std::unique_ptr<VarNode> target_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
    BinaryCondOp op_;
};

class JumpNode : public InstructionNode {
public:
    JumpNode(std::shared_ptr<Asm::Label> label) : target_label_(label) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "JUMP TO: " + std::to_string(target_label_->address_);
    }

    ASMCode to_asm() const override {
        ASMCode code;
        code.push_back(asm_jump(target_label_));
        return code;
    }
private:
    std::shared_ptr<Asm::Label> target_label_;
};

class JumpIfZeroNode : public InstructionNode {
public:
    JumpIfZeroNode(std::unique_ptr<ValueNode> condition, std::shared_ptr<Asm::Label> label) : 
        condition_(std::move(condition)), 
        target_label_(label) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "JUMP IF ZERO TO: " + std::to_string(target_label_->address_) + "\n" +
               condition_->to_string(level + 1);
    }   

    ASMCode to_asm() const override {
        ASMCode code;

        append_code(code, std::move(condition_->to_asm()));

        code.push_back(asm_jump_if_zero(target_label_));

        return code;
    }
private:
    std::unique_ptr<ValueNode> condition_;
    std::shared_ptr<Asm::Label> target_label_;
};

class JumpIfPositiveNode : public InstructionNode {
public:
    JumpIfPositiveNode(std::unique_ptr<ValueNode> condition, std::shared_ptr<Asm::Label> label) : 
        condition_(std::move(condition)), 
        target_label_(label) {}
    
    std::string to_string(int level = 0) const override {
        return util::pad(level) + "JUMP IF POSITIVE TO: " + std::to_string(target_label_->address_) + "\n" +
               condition_->to_string(level + 1);
    }

    ASMCode to_asm() const override {
        ASMCode code;

        append_code(code, std::move(condition_->to_asm()));

        code.push_back(asm_jump_if_positive(target_label_));

        return code;
    }
private:
    std::unique_ptr<ValueNode> condition_;
    std::shared_ptr<Asm::Label> target_label_;
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
        ASMCode code;
        auto label_loop = std::make_shared<Asm::Label>("LOOP_START");
        auto label_bit_zero = std::make_shared<Asm::Label>("BIT_ZERO");
        auto label_end = std::make_shared<Asm::Label>("LOOP_END");

        append_code(code, right_->to_asm());   // ra = right
        code.push_back(asm_swap(Register::RB)); // rb = right
        append_code(code, left_->to_asm());    // ra = left
        
        code.push_back(asm_reset(Register::RC)); // rc = 0 (acc)

        code.push_back(asm_label_marker(label_loop));
        code.push_back(asm_jump_if_zero(label_end));

        // Youngest ra bit test: rd = ra; ra = ra / 2 * 2; rd = rd - ra
        code.push_back(asm_reset(Register::RD));
        code.push_back(asm_swap(Register::RD));  // rd = 0
        code.push_back(asm_add(Register::RD));   // rd = ra
        code.push_back(asm_shift_right(Register::RA));
        code.push_back(asm_shift_left(Register::RA));
        code.push_back(asm_swap(Register::RD));  // ra = old_val, rd = shifted
        code.push_back(asm_subtract(Register::RD)); // ra = ra - rd (result: 1 or 0)

        code.push_back(asm_jump_if_zero(label_bit_zero)); // if rd == 0 skip addition
        code.push_back(asm_swap(Register::RC));
        code.push_back(asm_add(Register::RB));   // rc += rb
        code.push_back(asm_swap(Register::RC));

        code.push_back(asm_label_marker(label_bit_zero));
        // Prepare for next bit
        code.push_back(asm_reset(Register::RA));
        code.push_back(asm_add(Register::RD));
        code.push_back(asm_shift_right(Register::RA)); // ra >>= 1
        code.push_back(asm_shift_left(Register::RB));  // rb <<= 1
        code.push_back(asm_jump(label_loop));

        code.push_back(asm_label_marker(label_end));
        code.push_back(asm_reset(Register::RA));
        code.push_back(asm_add(Register::RC)); // Wynik do ra
        
        auto target_addr = SymbolTable::get_address(target_->get_name());
        code.push_back(asm_store(target_addr));
        return code;
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
        code.push_back(asm_halt());
        return code;
    }
private:
    std::vector<std::unique_ptr<InstructionNode>> instructions_;
};

} // namespace Tac