#pragma once

namespace AST {
class AssignmentNode;
class IdentifierNode;
class BinaryOpNode;
class WriteNode;
class ReadNode;
class ProgramNode;
class MainNode;
class CommandsNode;
class ConstantNode;
class DeclarationsNode;
class BinaryCondNode;
} // namespace AST

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(AST::IdentifierNode& node);
    virtual void visit(AST::ConstantNode& node);
    virtual void visit(AST::WriteNode& node);
    virtual void visit(AST::ReadNode& node);
    virtual void visit(AST::BinaryOpNode& node);
    virtual void visit(AST::AssignmentNode& node);
    virtual void visit(AST::CommandsNode& node);
    virtual void visit(AST::DeclarationsNode& node);
    virtual void visit(AST::MainNode& node);
    virtual void visit(AST::ProgramNode& node);
    virtual void visit(AST::BinaryCondNode& node);
};