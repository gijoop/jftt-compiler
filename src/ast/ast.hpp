#pragma once

#include <string>
#include <vector>
#include <memory>

#include <iostream>

#include "util.hpp"
#include "symbol_table.hpp"
#include "types.hpp"

#include "ast/visitor.hpp"

namespace AST {

class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_string(int level = 0) const = 0;
    virtual void accept(AstVisitor& visitor) = 0;
};

#define ACCEPT_VISITOR \
    void accept(AstVisitor& v) override { v.visit(*this); }

class CommandNode : public Node {
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

    void add_command(std::unique_ptr<CommandNode> cmd) {
        commands_.push_back(std::move(cmd));
    }

    const std::vector<std::unique_ptr<CommandNode>>& commands() const {
        return commands_;
    }

private:
    std::vector<std::unique_ptr<CommandNode>> commands_;
};

class ExpressionNode : public Node {};

class ValueNode : public ExpressionNode {};

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(
        BinaryOp op,
        std::unique_ptr<ValueNode> left, 
        std::unique_ptr<ValueNode> right) : 
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

    const std::unique_ptr<ValueNode>& left() const {
        return left_;
    }

    const std::unique_ptr<ValueNode>& right() const {
        return right_;
    }

    const BinaryOp op() const {
        return op_;
    }
private:
    BinaryOp op_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};

class BinaryCondNode : public Node {
public:
    BinaryCondNode(
        BinaryCondOp op,
        std::unique_ptr<ValueNode> left, 
        std::unique_ptr<ValueNode> right) :
        op_(op),
        left_(std::move(left)), 
        right_(std::move(right)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "BINARY_COND_OP: " + 
               util::to_string(op_) + "\n" +
               left_->to_string(level + 1) + "\n" +
               right_->to_string(level + 1);
    }

    const std::unique_ptr<ValueNode>& left() const {
        return left_;
    }

    const std::unique_ptr<ValueNode>& right() const {
        return right_;
    }

    const BinaryCondOp op() const {
        return op_;
    }
private:
    BinaryCondOp op_;
    std::unique_ptr<ValueNode> left_;
    std::unique_ptr<ValueNode> right_;
};


class IdentifierNode : public ValueNode {
public:
    IdentifierNode(const std::string& name) : name_(name) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "IDENTIFIER: " + name_;
    }

    const std::string& get_name() const { return name_; }

private:
    std::string name_;
};

class ConstantNode : public ValueNode {
public:
    ConstantNode(long long value) : value_(value) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "NUM: " + std::to_string(value_);
    }

    const long long value() const {
        return value_;
    }

private:
    long long value_;
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

class WriteNode : public CommandNode {
public:
    WriteNode(std::unique_ptr<ValueNode> expr) : expr_(std::move(expr)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "WRITE:\n" + expr_->to_string(level + 1);
    }

    const std::unique_ptr<ValueNode>& expr() const {
        return expr_;
    }

private:
    std::unique_ptr<ValueNode> expr_;
};

class ReadNode : public CommandNode {
public:
    ReadNode(std::unique_ptr<IdentifierNode> id) : id_(std::move(id)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "READ:\n" + id_->to_string(level + 1);
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

    const std::unique_ptr<MainNode>& main() const {
        return main_;
    }
private:
    std::unique_ptr<MainNode> main_;
};

} // namespace AST