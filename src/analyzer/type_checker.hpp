#pragma once

#include "ast/visitor.hpp"
#include "analyzer/semantic_error.hpp"
#include "ast/ast.hpp"
#include "symbol_table.hpp"

namespace AST {

class TypeChecker : public AstVisitor {
public:
    TypeChecker() = default;

    struct VarInfo {
        AST::ParamType type = AST::ParamType::DEFAULT;

        bool is_initialized = false;
    };

    std::string scoped(const std::string& name) const {
        return current_procedure_.empty() ? name : current_procedure_ + "." + name;
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
            vars_[scoped(arg.name)].type = arg.type;
            procedure_args_[current_procedure_].push_back(arg.type);
        }
    }

    void visit(ProcedureCallNode& node) override {
        auto it = procedure_args_.find(node.name());
        if (it == procedure_args_.end()) {
            throw std::runtime_error("Internal Error: Procedure '" + node.name() + "' not found in procedure arguments map.");
        }

        const auto& expected_args = it->second;
        const auto& provided_args = node.args()->arguments();

        for (size_t i = 0; i < expected_args.size(); ++i) {
            const auto& expected_type = expected_args[i];
            const auto& arg_name = provided_args[i];

            std::string full_name = scoped(arg_name);
            auto var_it = vars_.find(full_name);
            if (var_it == vars_.end()) {
                throw std::runtime_error("Internal Error: Variable '" + arg_name + "' not found in variable info map.");
            }

            const auto& actual_type = var_it->second.type;

            if (actual_type == AST::ParamType::CONST && expected_type != AST::ParamType::CONST) {
                throw SemanticError("Cannot pass constant variable '" + arg_name + "' to non-constant parameter.");
            }

            if (actual_type == AST::ParamType::UNDEFINED && expected_type == AST::ParamType::CONST) {
                throw SemanticError("Cannot pass undefined variable '" + arg_name + "' to constant parameter.");
            }

            if (expected_type == AST::ParamType::ARRAY && actual_type != AST::ParamType::ARRAY) {
                throw SemanticError("Expected array argument for parameter, but variable '" + arg_name + "' is not an array.");
            }

            if (expected_type != AST::ParamType::ARRAY && actual_type == AST::ParamType::ARRAY) {
                throw SemanticError("Expected non-array argument for parameter, but variable '" + arg_name + "' is an array.");
            }
        }
    }

    void visit(DeclarationsNode& node) override {
        for (const auto& decl : node.declarations()) {
            std::string full_name = scoped(decl.name);
            vars_[full_name].type = decl.is_array ? AST::ParamType::ARRAY : AST::ParamType::DEFAULT;
        }
    }

    void visit(AssignmentNode& node) override {
        node.expr()->accept(*this);

        auto& id_node = node.id();
        std::string full_name = scoped(id_node->get_name());
        
        if (vars_[full_name].type == AST::ParamType::CONST) {
            throw SemanticError("Cannot assign to constant variable '" + id_node->get_name() + "'.");
        }
        
        vars_[full_name].is_initialized = true;

        node.id()->accept(*this);
    }

    void visit(IdentifierNode& node) override {
        std::string full_name = scoped(node.get_name());

        if (vars_[full_name].type == AST::ParamType::ARRAY) {
            if (!node.get_index_name() && !node.get_index_value()) {
                throw SemanticError("Array '" + node.get_name() + "' must be indexed when used.");
            }
        } else {
            if (node.get_index_name() || node.get_index_value()) {
                throw SemanticError("Variable '" + node.get_name() + "' cannot be indexed when used.");
            }
        }

        if (vars_[full_name].type == AST::ParamType::UNDEFINED && !vars_[full_name].is_initialized) {
            throw SemanticError("Variable '" + node.get_name() + "' used before initialization.");
        }
    }

private:
    std::string current_procedure_;
    std::map<std::string, VarInfo> vars_;
    std::map<std::string, std::vector<AST::ParamType>> procedure_args_;
};

} // namespace AST