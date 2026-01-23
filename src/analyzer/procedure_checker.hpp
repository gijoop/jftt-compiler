#pragma once

#include "ast/visitor.hpp"
#include "analyzer/semantic_error.hpp"
#include "ast/ast.hpp"
#include "symbol_table.hpp"

namespace AST {

class ProcedureChecker : public AstVisitor {
public:
    ProcedureChecker() = default;

    bool is_procedure_declared(const std::string& name) {
        return declared_procedures_.find(name) != declared_procedures_.end();
    }

    bool declare_procedure(const std::string& name, int arg_count) {
        if (is_procedure_declared(name)) {
            return false;
        }
        declared_procedures_[name] = arg_count;
        return true;
    }

    void visit(ProcedureCallNode& node) override {
        if (node.name() == current_procedure_) {
            throw SemanticError("Recursive call detected in procedure " + current_procedure_);
        }
        if (!is_procedure_declared(node.name())) {
            throw SemanticError("Call to undeclared procedure " + util::quote(node.name()));
        }

        int expected_args = declared_procedures_[node.name()];
        int given_args = static_cast<int>(node.args()->arguments().size());
        if (given_args != expected_args) {
            throw SemanticError("Incorrect number of arguments in call to " + util::quote(node.name()) +
                                ". Expected " + std::to_string(expected_args) + ", got " +
                                std::to_string(given_args) + ".");
        }

        node.args()->accept(*this);
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

    void visit(ProcHeadNode& node) override {
        auto proc_name = node.name();
        int arg_count = node.args_decl()->arguments().size();

        if (!declare_procedure(proc_name, arg_count)) {
            throw SemanticError("Double declaration of procedure " + util::quote(proc_name));
        }
    }

private:
    std::string current_procedure_;
    std::map<std::string, int> declared_procedures_; // procedure_name -> arg_count
};

} // namespace AST