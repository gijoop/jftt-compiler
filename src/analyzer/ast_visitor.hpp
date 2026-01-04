#pragma once

namespace LangAST {

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

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(IdentifierNode& node);
    virtual void visit(ConstantNode& node);
    virtual void visit(WriteNode& node);
    virtual void visit(ReadNode& node);
    virtual void visit(BinaryOpNode& node);
    virtual void visit(AssignmentNode& node);
    virtual void visit(CommandsNode& node);
    virtual void visit(DeclarationsNode& node);
    virtual void visit(MainNode& node);
    virtual void visit(ProgramNode& node);
};
}