#pragma once

#include <string>
#include <vector>
#include <memory>

#include "util.hpp"
#include "symbol_table.hpp"
#include "operator.hpp"

#include "analyzer/ast_visitor.hpp"
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
inline auto tac_program(std::vector<std::unique_ptr<Tac::InstructionNode>> instructions) {
    return make_node<Tac::ProgramNode>(std::move(instructions));
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
    virtual void accept(AstVisitor& visitor) = 0;
};

#define ACCEPT_VISITOR \
    void accept(AstVisitor& v) override { v.visit(*this); }

class CommandNode : public Node {
public:
    virtual InstructionList to_tac_instructions() const = 0;
};

class CommandsNode : public Node {
public:
    CommandsNode(std::vector<std::unique_ptr<CommandNode>> commands) : 
        commands_(std::move(commands)) {}
    
    CommandsNode(std::unique_ptr<CommandNode> command) {
        commands_.push_back(std::move(command));
    }

    ACCEPT_VISITOR
    
    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "COMMANDS:\n";
        for (const auto& cmd : commands_) {
            result += cmd->to_string(level + 1) + "\n";
        }
        return result;
    }

    InstructionList to_tac_instructions() const {
        InstructionList instrs;
        for (const auto& cmd : commands_) {
            auto cmd_instrs = cmd->to_tac_instructions();
            append_instr(instrs, std::move(cmd_instrs));
        }
        return instrs;
    }

    void add_command(std::unique_ptr<CommandNode> cmd) {
        commands_.push_back(std::move(cmd));
    }

    const std::vector<std::unique_ptr<CommandNode>>& commands() const {
        return commands_;
    }

private:
    std::vector<std::unique_ptr<CommandNode>> commands_;
};

class ExpressionNode : public Node {
public:
    virtual TacExpressionResult to_tac_expression() const = 0;
};

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(
        BinaryOp op,
        std::unique_ptr<ExpressionNode> left, 
        std::unique_ptr<ExpressionNode> right) : 
        op_(op),
        left_(std::move(left)), 
        right_(std::move(right)) {}
    
    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "BINARY_OP: " + 
               util::to_string(op_) + "\n" +
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
            case BinaryOp::ADD:
                instrs.push_back(
                    make_node<Tac::AddNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case BinaryOp::SUB:
                instrs.push_back(
                    make_node<Tac::SubtractNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case BinaryOp::MUL:
                instrs.push_back(
                    make_node<Tac::MultiplyNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case BinaryOp::DIV:
                instrs.push_back(
                    make_node<Tac::DivideNode>(
                        tac_var(temp_name),
                        std::move(left_res.result),
                        std::move(right_res.result)
                    )
                );
                break;
            case BinaryOp::MOD:
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

    const std::unique_ptr<ExpressionNode>& left() const {
        return left_;
    }

    const std::unique_ptr<ExpressionNode>& right() const {
        return right_;
    }

    const BinaryOp op() const {
        return op_;
    }
private:
    BinaryOp op_;
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
};


class IdentifierNode : public ExpressionNode {
public:
    IdentifierNode(const std::string& name) : name_(name) {}

    ACCEPT_VISITOR

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

class DeclarationsNode : public Node {
public:
    DeclarationsNode(std::vector<std::unique_ptr<IdentifierNode>> ids) : 
        ids_(std::move(ids)) {}

    DeclarationsNode(std::unique_ptr<IdentifierNode> id) {
        ids_.push_back(std::move(id));
    }

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "DECLARATIONS:\n";
        for (const auto& id : ids_) {
            result += id->to_string(level + 1) + "\n";
        }
        return result;
    }

    void add_declaration(std::unique_ptr<IdentifierNode> id) {
        ids_.push_back(std::move(id));
    }

    std::vector<std::unique_ptr<IdentifierNode>>& ids() {
        return ids_;
    }
private:
    std::vector<std::unique_ptr<IdentifierNode>> ids_;
};

class ConstantNode : public ExpressionNode {
public:
    ConstantNode(long long value) : value_(value) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "NUM: " + std::to_string(value_);
    }

    TacExpressionResult to_tac_expression() const override {
        return {
            InstructionList{},
            tac_const(value_)
        };
    }

    const long long value() const {
        return value_;
    }

private:
    long long value_;
};

// class VariableNode : public ExpressionNode {
// public:
//     VariableNode(std::unique_ptr<IdentifierNode> id) : id_(std::move(id)) {}

//     std::string to_string(int level = 0) const override {
//         return util::pad(level) + "VARIABLE:\n" + id_->to_string(level + 1);
//     }

//     TacExpressionResult to_tac_expression() const override {
//         return {
//             InstructionList{},
//             tac_var(id_->get_name())
//         };
//     }
// private:
//     std::unique_ptr<IdentifierNode> id_;
// };

class WriteNode : public CommandNode {
public:
    WriteNode(std::unique_ptr<ExpressionNode> expr) : expr_(std::move(expr)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "WRITE:\n" + expr_->to_string(level + 1);
    }

    InstructionList to_tac_instructions() const override {
        auto res = expr_->to_tac_expression();
        InstructionList instrs = std::move(res.instructions);
        
        instrs.push_back(tac_write(std::move(res.result)));
        return instrs;
    }

    const std::unique_ptr<ExpressionNode>& expr() const {
        return expr_;
    }

private:
    std::unique_ptr<ExpressionNode> expr_;
};

class ReadNode : public CommandNode {
public:
    ReadNode(std::unique_ptr<IdentifierNode> id) : id_(std::move(id)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "READ:\n" + id_->to_string(level + 1);
    }

    InstructionList to_tac_instructions() const override {
        InstructionList instrs;
        
        instrs.push_back(make_node<Tac::ReadNode>(tac_var(id_->get_name())));
        
        return instrs;
    }

    const std::unique_ptr<IdentifierNode>& id() const {
        return id_;
    }
private:
    std::unique_ptr<IdentifierNode> id_;
};

class AssignmentNode : public CommandNode {
public:
    AssignmentNode(std::unique_ptr<IdentifierNode> id, std::unique_ptr<ExpressionNode> expr) : 
        id_(std::move(id)), 
        expr_(std::move(expr)) {}

    ACCEPT_VISITOR
    
    std::string to_string(int level = 0) const override {
        return util::pad(level) + "ASSIGNMENT:\n" +
               id_->to_string(level + 1) + "\n" +
               expr_->to_string(level + 1);
    }

    InstructionList to_tac_instructions() const override {
        auto res = expr_->to_tac_expression();
        InstructionList instrs = std::move(res.instructions);
        instrs.push_back(
            make_node<Tac::CopyNode>(
                tac_var(id_->get_name()),
                std::move(res.result)
            )
        );
        return instrs;
    }

    const std::unique_ptr<IdentifierNode>& id() const {
        return id_;
    }

    const std::unique_ptr<ExpressionNode>& expr() const {
        return expr_;
    }
private:
    std::unique_ptr<IdentifierNode> id_;
    std::unique_ptr<ExpressionNode> expr_;
};

class MainNode : public Node {
public:
    MainNode(std::unique_ptr<DeclarationsNode> decls, std::unique_ptr<CommandsNode> cmds) : 
        decls_(std::move(decls)), 
        commands_(std::move(cmds)) {}

    MainNode(std::unique_ptr<CommandsNode> cmds) : 
        decls_(nullptr), 
        commands_(std::move(cmds)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "MAIN:\n";
        if (decls_) result += decls_->to_string(level + 1) + "\n";
        if (commands_) result += commands_->to_string(level + 1);
        return result;
    }

    InstructionList to_tac() const {
        InstructionList instrs;
        auto cmds_instrs = commands_->to_tac_instructions();
        append_instr(instrs, std::move(cmds_instrs));
        return instrs;
    }

    const std::unique_ptr<DeclarationsNode>& declarations() const {
        return decls_;
    }

    const std::unique_ptr<CommandsNode>& commands() const {
        return commands_;
    }

private:
    std::unique_ptr<DeclarationsNode> decls_;
    std::unique_ptr<CommandsNode> commands_;
};

class ProgramNode : public Node {
public:
    ProgramNode(std::unique_ptr<MainNode> main) : main_(std::move(main)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "PROGRAM:\n" + main_->to_string(level + 1);
    }

    std::unique_ptr<Tac::ProgramNode> to_tac_program() const {
        return tac_program(
            std::move(main_->to_tac())
        );
    }

    const std::unique_ptr<MainNode>& main() const {
        return main_;
    }
private:
    std::unique_ptr<MainNode> main_;
};

} // namespace LangAST