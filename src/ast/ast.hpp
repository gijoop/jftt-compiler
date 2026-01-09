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
    DeclarationsNode(std::vector<std::string> names) : 
        names_(std::move(names)) {}

    DeclarationsNode(std::string name) {
        names_.push_back(std::move(name));
    }

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "DECLARATIONS:\n";
        for (const auto& name : names_) {
            result += util::pad(level + 1) + name + "\n";
        }
        return result;
    }

    void add_declaration(std::string name) {
        names_.push_back(std::move(name));
    }

    std::vector<std::string>& names() {
        return names_;
    }
private:
    std::vector<std::string> names_;
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

class IfNode : public CommandNode {
public:
    IfNode(
        std::unique_ptr<BinaryCondNode> condition, 
        std::unique_ptr<CommandsNode> then_commands,
        std::optional<std::unique_ptr<CommandsNode>> else_commands = std::nullopt) :
        condition_(std::move(condition)),
        then_commands_(std::move(then_commands)),
        else_commands_(std::move(else_commands)) {}
    
    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "IF:\n" +
                             condition_->to_string(level + 1) + "\n" +
                             util::pad(level) + "THEN:\n" +
                             then_commands_->to_string(level + 1);
        if (else_commands_) {
            result += "\n" + util::pad(level) + "ELSE:\n" +
                      (*else_commands_)->to_string(level + 1);
        }
        return result;
    }
    
    const std::unique_ptr<BinaryCondNode>& condition() const {
        return condition_;
    }

    const std::unique_ptr<CommandsNode>& then_commands() const {
        return then_commands_;
    }

    const std::optional<std::unique_ptr<CommandsNode>>& else_commands() const {
        return else_commands_;
    }

private:
    std::unique_ptr<BinaryCondNode> condition_;
    std::unique_ptr<CommandsNode> then_commands_;
    std::optional<std::unique_ptr<CommandsNode>> else_commands_;
};

class WhileNode : public CommandNode {
public:
    WhileNode(
        std::unique_ptr<BinaryCondNode> condition, 
        std::unique_ptr<CommandsNode> commands) :
        condition_(std::move(condition)),
        commands_(std::move(commands)) {}
    
    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "WHILE:\n" +
                             condition_->to_string(level + 1) + "\n" +
                             util::pad(level) + "DO:\n" +
                             commands_->to_string(level + 1);
        return result;
    }

    const std::unique_ptr<BinaryCondNode>& condition() const {
        return condition_;
    }

    const std::unique_ptr<CommandsNode>& commands() const {
        return commands_;
    }

private:
    std::unique_ptr<BinaryCondNode> condition_;
    std::unique_ptr<CommandsNode> commands_;
};

class RepeatNode : public CommandNode {
public:
    RepeatNode(
        std::unique_ptr<BinaryCondNode> condition, 
        std::unique_ptr<CommandsNode> commands) :
        condition_(std::move(condition)),
        commands_(std::move(commands)) {}
    
    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "REPEAT:\n" +
                             commands_->to_string(level + 1) + "\n" +
                             util::pad(level) + "UNTIL:\n" +
                             condition_->to_string(level + 1);
        return result;
    }

    const std::unique_ptr<BinaryCondNode>& condition() const {
        return condition_;
    }

    const std::unique_ptr<CommandsNode>& commands() const {
        return commands_;
    }

private:
    std::unique_ptr<BinaryCondNode> condition_;
    std::unique_ptr<CommandsNode> commands_;
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

class ArgumentsDeclNode : public Node {
public:
    ArgumentsDeclNode(std::vector<std::string> args) : args_(std::move(args)) {}

    ArgumentsDeclNode(std::string arg) {
        args_.push_back(std::move(arg));
    }

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "ARGS_DECL:\n";
        for (const auto& arg : args_) {
            result += util::pad(level + 1) + arg + "\n";
        }
        return result;
    }

    void add_argument(std::string arg) {
        args_.push_back(std::move(arg));
    }

    std::vector<std::string>& arguments() {
        return args_;
    }
private:
    std::vector<std::string> args_;
};

class ArgumentsNode : public Node {
public:
    ArgumentsNode(std::vector<std::string> args) : args_(std::move(args)) {}

    ArgumentsNode(std::string arg) {
        args_.push_back(std::move(arg));
    }

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "ARGS:\n";
        for (const auto& arg : args_) {
            result += util::pad(level + 1) + arg + "\n";
        }
        return result;
    }

    void add_argument(std::string arg) {
        args_.push_back(std::move(arg));
    }

    std::vector<std::string>& arguments() {
        return args_;
    }
private:
    std::vector<std::string> args_;
};

class ProcHeadNode : public Node {
public:
    ProcHeadNode(
        std::string name,
        std::unique_ptr<ArgumentsDeclNode> args_decl) :
        name_(std::move(name)),
        args_decl_(std::move(args_decl)) {}
        
    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "PROC_HEAD: " + name_ + "\n" +
               args_decl_->to_string(level + 1);
    }

    const std::string& name() const {
        return name_;
    }

    const std::unique_ptr<ArgumentsDeclNode>& args_decl() const {
        return args_decl_;
    }

private:
    std::string name_;
    std::unique_ptr<ArgumentsDeclNode> args_decl_;
};

class ProcedureNode : public Node {
public:
    ProcedureNode(
        std::unique_ptr<ProcHeadNode> head, 
        std::optional<std::unique_ptr<DeclarationsNode>> decls,
        std::unique_ptr<CommandsNode> cmds) :
        head_(std::move(head)),
        decls_(std::move(decls)),
        commands_(std::move(cmds)) {}

    ProcedureNode(
        std::unique_ptr<ProcHeadNode> head, 
        std::unique_ptr<CommandsNode> cmds) :
        head_(std::move(head)),
        decls_(std::nullopt),
        commands_(std::move(cmds)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "PROCEDURE:\n" +
               head_->to_string(level + 1) + "\n";
        if (decls_)  result += (*decls_)->to_string(level + 1) + "\n";
        result += commands_->to_string(level + 1);
        return result;
    }

    const std::unique_ptr<ProcHeadNode>& head() const {
        return head_;
    }

    const std::optional<std::unique_ptr<DeclarationsNode>>& declarations() const {
        return decls_;
    }

    const std::unique_ptr<CommandsNode>& commands() const {
        return commands_;
    }
private:
    std::unique_ptr<ProcHeadNode> head_;
    std::optional<std::unique_ptr<DeclarationsNode>> decls_;
    std::unique_ptr<CommandsNode> commands_;
};

class ProceduresNode : public Node {
public:
    ProceduresNode() = default;

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "PROCEDURES:\n";
        for (const auto& proc : procedures_) {
            result += proc->to_string(level + 1) + "\n";
        }
        return result;
    }

    void add_procedure(std::unique_ptr<ProcedureNode> proc) {
        procedures_.push_back(std::move(proc));
    }

    const std::vector<std::unique_ptr<ProcedureNode>>& procedures() const {
        return procedures_;
    }

private:
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;
};

class ProcedureCallNode : public CommandNode {
public:
    ProcedureCallNode(
        const std::string& name, 
        std::unique_ptr<ArgumentsNode> args) : 
        name_(name), 
        args_(std::move(args)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "PROC_CALL: " + name_ + "\n" +
               args_->to_string(level + 1);
    }

    const std::string& name() const {
        return name_;
    }
    
    const std::unique_ptr<ArgumentsNode>& args() const {
        return args_;
    }

private:
    std::string name_;
    std::unique_ptr<ArgumentsNode> args_;
};

class MainNode : public Node {
public:
    MainNode(
        std::optional<std::unique_ptr<DeclarationsNode>> decls,
        std::unique_ptr<CommandsNode> cmds) : 
        decls_(std::move(decls)), 
        commands_(std::move(cmds)) {}

    MainNode(std::unique_ptr<CommandsNode> cmds) : 
        decls_(std::nullopt), 
        commands_(std::move(cmds)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        std::string result = util::pad(level) + "MAIN:\n";
        if (decls_) result += (*decls_)->to_string(level + 1) + "\n";
        if (commands_) result += commands_->to_string(level + 1);
        return result;
    }

    const std::optional<std::unique_ptr<DeclarationsNode>>& declarations() const {
        return decls_;
    }

    const std::unique_ptr<CommandsNode>& commands() const {
        return commands_;
    }

private:
    std::optional<std::unique_ptr<DeclarationsNode>> decls_;
    std::unique_ptr<CommandsNode> commands_;
};

class ProgramNode : public Node {
public:
    ProgramNode(std::unique_ptr<ProceduresNode> procedures, std::unique_ptr<MainNode> main) : procedures_(std::move(procedures)), main_(std::move(main)) {}

    ACCEPT_VISITOR

    std::string to_string(int level = 0) const override {
        return util::pad(level) + "PROGRAM:\n" + procedures_->to_string(level + 1) + "\n" + main_->to_string(level + 1);
    }

    const std::unique_ptr<ProceduresNode>& procedures() const {
        return procedures_;
    }

    const std::unique_ptr<MainNode>& main() const {
        return main_;
    }
private:
    std::unique_ptr<ProceduresNode> procedures_;
    std::unique_ptr<MainNode> main_;
};

} // namespace AST