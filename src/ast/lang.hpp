#pragma once

#include <string>
#include <vector>
#include <memory>

#include "util.hpp"
#include "symbol_table.hpp"
#include "ast/tac.hpp"

namespace LangAST {

using InstructionList = std::vector<std::unique_ptr<Tac::InstructionNode>>;

struct TacExpressionResult {
    InstructionList instructions;
    std::unique_ptr<Tac::ValueNode> result;
};

inline void append_instr(InstructionList& dest, InstructionList src) {
    dest.insert(
        dest.end(),
        std::make_move_iterator(src.begin()),
        std::make_move_iterator(src.end())
    );
}

template <typename T, typename... Args>
auto make_node(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}


inline auto tac_write(std::unique_ptr<Tac::ValueNode> val) {
    return make_node<Tac::WriteNode>(std::move(val));
}
inline auto tac_program(std::unique_ptr<Tac::FunctionNode> func) {
    return make_node<Tac::ProgramNode>(std::move(func));
}
inline auto tac_function(const std::string& id, InstructionList instructions) {
    return make_node<Tac::FunctionNode>(id, std::move(instructions));
}
inline auto tac_var(const std::string& name) {
    return make_node<Tac::VarNode>(name);
}
inline auto tac_const(long long value) {
    return make_node<Tac::ConstantNode>(value);
}

class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_string(int level = 0) const = 0;
};

class CommandNode : public Node {
public:
    virtual InstructionList to_tac_instructions() const = 0;
};

class ExpressionNode : public Node {
public:
    virtual TacExpressionResult to_tac_expression() const = 0;
};

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(
        Tac::BinaryOp op,
        std::unique_ptr<ExpressionNode> left, 
        std::unique_ptr<ExpressionNode> right) : 
        op_(op),
        left_(std::move(left)), 
        right_(std::move(right)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "BINARY_OP: " + 
               (op_ == Tac::BinaryOp::ADD ? "+" :
                op_ == Tac::BinaryOp::SUB ? "-" :
                op_ == Tac::BinaryOp::MUL ? "*" :
                op_ == Tac::BinaryOp::DIV ? "/" :
                op_ == Tac::BinaryOp::MOD ? "%" : "?") + "\n" +
               left_->to_string(level + 1) + "\n" +
               right_->to_string(level + 1);
    } 

    TacExpressionResult to_tac_expression() const override {
        auto left_res = left_->to_tac_expression();
        auto right_res = right_->to_tac_expression();

        InstructionList instrs;

        append_instr(instrs, std::move(left_res.instructions));
        append_instr(instrs, std::move(right_res.instructions));

        std::string temp_name = SymbolTable::get_temp_var_name();

        switch (op_) {
            case Tac::BinaryOp::ADD:
                instrs.push_back(
                    make_node<Tac::AddNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case Tac::BinaryOp::SUB:
                instrs.push_back(
                    make_node<Tac::SubtractNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case Tac::BinaryOp::MUL:
                instrs.push_back(
                    make_node<Tac::MultiplyNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case Tac::BinaryOp::DIV:
                instrs.push_back(
                    make_node<Tac::DivideNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case Tac::BinaryOp::MOD:
                instrs.push_back(
                    make_node<Tac::ModuloNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
        }

        return {
            std::move(instrs),
            std::make_unique<Tac::VarNode>(temp_name)
        };
    }

private:
    Tac::BinaryOp op_;
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
};


class IdentifierNode : public ExpressionNode {
public:
    IdentifierNode(const std::string& name) : name_(name) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "IDENTIFIER: " + name_;
    }

    const std::string& get_name() const { return name_; }

    TacExpressionResult to_tac_expression() const override {
        return {
            InstructionList{},
            tac_var(name_)
        };
    }

private:
    std::string name_;
};

class ConstantNode : public ExpressionNode {
public:
    ConstantNode(long long value) : value_(value) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "NUM: " + std::to_string(value_);
    }

    TacExpressionResult to_tac_expression() const override {
        return {
            InstructionList{},
            tac_const(value_)
        };
    }

private:
    long long value_;
};

class WriteNode : public CommandNode {
public:
    WriteNode(std::unique_ptr<ExpressionNode> expr) : expr_(std::move(expr)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "WRITE:\n" + expr_->to_string(level + 1);
    }

    InstructionList to_tac_instructions() const override {
        auto res = expr_->to_tac_expression();
        InstructionList instrs = std::move(res.instructions);
        
        instrs.push_back(tac_write(std::move(res.result)));
        return instrs;
    }

private:
    std::unique_ptr<ExpressionNode> expr_;
};

class ProcedureNode : public Node {
public:
    ProcedureNode(std::unique_ptr<IdentifierNode> id, std::unique_ptr<CommandNode> cmd) : 
        id_(std::move(id)), 
        cmd_(std::move(cmd)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "PROCEDURE:\n" \
               + id_->to_string(level + 1) + "\n" \
               + cmd_->to_string(level + 1);
    }

    virtual std::unique_ptr<Tac::FunctionNode> to_tac_function() const {
        return tac_function(
            id_->get_name(), 
            cmd_->to_tac_instructions()
        );
    }

private:
    std::unique_ptr<IdentifierNode> id_;
    std::unique_ptr<CommandNode> cmd_;
};

class ProgramNode : public Node {
public:
    ProgramNode(std::unique_ptr<ProcedureNode> proc) : proc_(std::move(proc)) {}

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "PROGRAM:\n" + proc_->to_string(level + 1);
    }

    virtual std::unique_ptr<Tac::ProgramNode> to_tac_program() const {
        return tac_program(
            proc_->to_tac_function()
        );
    }

private:
    std::unique_ptr<ProcedureNode> proc_;
};

} // namespace LangAST