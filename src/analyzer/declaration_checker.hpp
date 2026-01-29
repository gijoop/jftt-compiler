#pragma once

#include "ast/visitor.hpp"
#include "analyzer/semantic_error.hpp"
#include "ast/ast.hpp"
#include "symbol_table.hpp"

using namespace util;

namespace AST {

class DeclarationChecker : public AstVisitor {
public:
    DeclarationChecker() = default;

    std::string scoped(const std::string& name) const {
        return current_procedure_.empty() ? name : current_procedure_ + "." + name;
    }

    bool is_variable_declared(const std::string& name) {
        return declared_variables_.find(name) != declared_variables_.end();
    }

    bool declare_variable(const std::string& name) {
        if (is_variable_declared(name)) {
            return false;
        }
        declared_variables_.insert(name);
        return true;
    }

    bool undeclare_variable(const std::string& name) {
        return declared_variables_.erase(name) > 0;
    }

    void visit(DeclarationsNode& node) override {
        for (auto& decls : node.declarations()) {
            std::string name = decls.name;
            std::string full_name;
            std::string procedure_error = "";
            if (!current_procedure_.empty()) {
                full_name = current_procedure_ + "." + name;
                procedure_error = " in procedure " + quote(current_procedure_);
            } else {
                full_name = name;
            }
            if (!declare_variable(full_name)) {
                throw SemanticError("Double declaration of " + quote(name) + procedure_error);
            }

            if (decls.is_array) {
                if (decls.start_index > decls.end_index) {
                    throw SemanticError("Invalid array bounds for " + quote(name) + procedure_error);
                }
            }
        }
    }

    void visit(IdentifierNode& node) override {
        if (!is_variable_declared(scoped(node.get_name()))) {
            throw SemanticError("Variable not declared " + quote(node.get_name()));
        }
    }

    void visit(ArgumentsNode& node) override {
        for (const auto& arg : node.arguments()) {
            if (!is_variable_declared(scoped(arg))) {
                throw SemanticError("Variable not declared " + quote(arg));
            }
        }
    }

    void visit(ProcedureNode& node) override {
        current_procedure_ = node.head()->name();

        node.head()->accept(*this);

        if (node.declarations()) {
            (*node.declarations())->accept(*this);
        }

        node.commands()->accept(*this);

        current_procedure_.clear();
    }

    void visit(ArgumentsDeclNode& node) override {
        for (const auto& arg : node.arguments()) {
            if (!declare_variable(scoped(arg.name))) {
                throw SemanticError("Double declaration of parameter " + quote(arg.name) + " in procedure " + quote(current_procedure_));
            }
        }
    }

    void visit(ForNode& node) override {
        node.start_val()->accept(*this);
        node.end_val()->accept(*this);

        if (!declare_variable(scoped(node.iterator()))) {
            throw SemanticError("Double declaration of " + quote(node.iterator()));
        }

        iterators_.insert(scoped(node.iterator()));
        node.commands()->accept(*this);

        iterators_.erase(scoped(node.iterator()));
        undeclare_variable(scoped(node.iterator()));
    }

    void visit(AssignmentNode& node) override {
        std::string var_name = node.id()->get_name();
        if (iterators_.find(scoped(var_name)) != iterators_.end()) {
            throw SemanticError("Cannot assign to loop iterator " + quote(var_name));
        }
        node.id()->accept(*this);
        node.expr()->accept(*this);
    }

    void visit(ReadNode& node) override {
        std::string var_name = node.id()->get_name();
        if (iterators_.find(scoped(var_name)) != iterators_.end()) {
            throw SemanticError("Cannot read into loop iterator " + quote(var_name));
        }
        node.id()->accept(*this);
    }
private:
    std::string current_procedure_;
    std::unordered_set<std::string> declared_variables_;
    std::unordered_set<std::string> iterators_;
};

} // namespace AST