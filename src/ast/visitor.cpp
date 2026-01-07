#include "ast/visitor.hpp"
#include "ast/ast.hpp"


void AstVisitor::visit(AST::IdentifierNode& node) {};
void AstVisitor::visit(AST::ConstantNode& node) {};

void AstVisitor::visit(AST::WriteNode& node) {
    node.expr()->accept(*this);
}

void AstVisitor::visit(AST::ReadNode& node) {
    node.id()->accept(*this);
}

void AstVisitor::visit(AST::BinaryOpNode& node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void AstVisitor::visit(AST::BinaryCondNode& node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void AstVisitor::visit(AST::AssignmentNode& node) {
    node.id()->accept(*this);
    node.expr()->accept(*this);
}

void AstVisitor::visit(AST::CommandsNode& node) {
    for (auto& cmd : node.commands()) {
        cmd->accept(*this);
    }
}

void AstVisitor::visit(AST::DeclarationsNode& node) {
    for (auto& id : node.ids()) {
        id->accept(*this);
    }
}

void AstVisitor::visit(AST::IfNode& node) {
    node.condition()->accept(*this);
    node.then_commands()->accept(*this);
    if (node.else_commands()) {
        (*node.else_commands())->accept(*this);
    }
}

void AstVisitor::visit(AST::WhileNode& node) {
    node.condition()->accept(*this);
    node.commands()->accept(*this);
}

void AstVisitor::visit(AST::RepeatNode& node) {
    node.commands()->accept(*this);
    node.condition()->accept(*this);
}

void AstVisitor::visit(AST::MainNode& node) {
    if (node.declarations()) node.declarations()->accept(*this);
    if (node.commands()) node.commands()->accept(*this);
}

void AstVisitor::visit(AST::ProgramNode& node) {
    node.main()->accept(*this);
}
