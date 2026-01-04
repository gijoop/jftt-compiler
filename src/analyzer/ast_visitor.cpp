#include "ast_visitor.hpp"
#include "../ast/lang.hpp"

namespace LangAST {

void AstVisitor::visit(IdentifierNode& node) {};
void AstVisitor::visit(ConstantNode& node) {};

void AstVisitor::visit(WriteNode& node) {
    node.expr()->accept(*this);
}

void AstVisitor::visit(ReadNode& node) {
    node.id()->accept(*this);
}

void AstVisitor::visit(BinaryOpNode& node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void AstVisitor::visit(BinaryCondNode& node) {
    node.left()->accept(*this);
    node.right()->accept(*this);
}

void AstVisitor::visit(AssignmentNode& node) {
    node.id()->accept(*this);
    node.expr()->accept(*this);
}

void AstVisitor::visit(CommandsNode& node) {
    for (auto& cmd : node.commands()) {
        cmd->accept(*this);
    }
}

void AstVisitor::visit(DeclarationsNode& node) {
    for (auto& id : node.ids()) {
        id->accept(*this);
    }
}

void AstVisitor::visit(MainNode& node) {
    if (node.declarations()) node.declarations()->accept(*this);
    if (node.commands()) node.commands()->accept(*this);
}

void AstVisitor::visit(ProgramNode& node) {
    node.main()->accept(*this);
}

}
