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
class IfNode;
class WhileNode;
class RepeatNode;
class ProcHeadNode;
class ProcedureNode;
class ProceduresNode;
class ProcedureCallNode;
class ArgumentsNode;
class ArgumentsDeclNode;
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
    virtual void visit(AST::IfNode& node);
    virtual void visit(AST::WhileNode& node);
    virtual void visit(AST::RepeatNode& node);
    virtual void visit(AST::ProcHeadNode& node);
    virtual void visit(AST::ProcedureNode& node);
    virtual void visit(AST::ProceduresNode& node);
    virtual void visit(AST::ProcedureCallNode& node);
    virtual void visit(AST::ArgumentsNode& node);
    virtual void visit(AST::ArgumentsDeclNode& node);
};