#pragma once

#include "ast/visitor.hpp"
#include "analyzer/semantic_error.hpp"
#include "ast/ast.hpp"
#include "symbol_table.hpp"

namespace AST {

class DeclarationChecker : public AstVisitor {
public:
    DeclarationChecker() = default;

    // Rejestrujemy zmienne
    void visit(DeclarationsNode& node) override {
        for (auto& id : node.ids()) {
            if (!SymbolTable::declare(id->get_name())) {
                throw SemanticError("Double declaration of " + id->get_name());
            }
        }
    }

    // Sprawdzamy użycie
    void visit(IdentifierNode& node) override {
        if (!SymbolTable::is_declared(node.get_name())) {
            throw SemanticError("Variable not declared: " + node.get_name());
        }
    }
};

} // namespace AST