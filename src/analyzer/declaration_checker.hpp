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
        for (auto& name : node.names()) {
            std::string full_name;
            if (!current_procedure.empty()) {
                full_name = current_procedure + "." + name;
            } else {
                full_name = name;
            }
            if (!SymbolTable::declare(full_name)) {
                throw SemanticError("Double declaration of " + full_name);
            }
        }
    }

    // Sprawdzamy użycie
    void visit(IdentifierNode& node) override {
        std::string full_name;
        if (!current_procedure.empty()) {
            full_name = current_procedure + "." + node.get_name();
        } else {
            full_name = node.get_name();
        }
        if (!SymbolTable::is_declared(full_name)) {
            throw SemanticError("Variable not declared: " + full_name);
        }
    }

    void visit(ProcedureNode& node) override {
        current_procedure = node.head()->name();

        for (const auto& arg : node.head()->args_decl()->arguments()) {
            std::string ref_name;
            if (!current_procedure.empty()) {
                ref_name = current_procedure + "." + arg;
            } else {
                ref_name = arg;
            }
            if (!SymbolTable::declare(ref_name)) {
                throw SemanticError("Double declaration of parameter " + arg + " in procedure " + current_procedure);
            }
        }

        // Sprawdzamy ciało procedury
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