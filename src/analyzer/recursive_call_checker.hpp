#pragma once

#include "ast/visitor.hpp"
#include "analyzer/semantic_error.hpp"
#include "ast/ast.hpp"
#include "symbol_table.hpp"

namespace AST {

class RecursiveCallChecker : public AstVisitor {
public:
    RecursiveCallChecker() = default;

    void visit(ProcedureCallNode& node) override {
        if (node.name() == current_procedure) {
            throw SemanticError("Recursive call detected in procedure " + current_procedure);
        }
    }

    void visit(ProcedureNode& node) override {
        current_procedure = node.head()->name();

        if (node.declarations()) {
            (*node.declarations())->accept(*this);
        }

        node.commands()->accept(*this);

        current_procedure.clear();
    }
private:
    std::string current_procedure;
};

} // namespace AST