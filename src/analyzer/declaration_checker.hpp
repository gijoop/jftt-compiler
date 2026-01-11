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
        }
    }

    void visit(IdentifierNode& node) override {
        std::string full_name;
        if (!current_procedure_.empty()) {
            full_name = current_procedure_ + "." + node.get_name();
        } else {
            full_name = node.get_name();
        }
        if (!is_variable_declared(full_name)) {
            throw SemanticError("Variable not declared " + quote(full_name));
        }
    }

    void visit(ArgumentsNode& node) override {
        for (const auto& arg : node.arguments()) {
            std::string full_name;
            if (!current_procedure_.empty()) {
                full_name = current_procedure_ + "." + arg;
            } else {
                full_name = arg;
            }
            if (!is_variable_declared(full_name)) {
                throw SemanticError("Variable not declared " + quote(full_name));
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
            std::string ref_name;
            if (!current_procedure_.empty()) {
                ref_name = current_procedure_ + "." + arg;
            } else {
                ref_name = arg;
            }
            if (!declare_variable(ref_name)) {
                throw SemanticError("Double declaration of parameter " + quote(arg) + " in procedure " + quote(current_procedure_));
            }
        }
    }

    void visit(ForNode& node) override {
        node.start_val()->accept(*this);
        node.end_val()->accept(*this);

        if (!declare_variable(node.iterator())) {
            throw SemanticError("Double declaration of " + quote(node.iterator()));
        }

        node.commands()->accept(*this);
    }
private:
    std::string current_procedure_;
    std::unordered_set<std::string> declared_variables_;
};

} // namespace AST