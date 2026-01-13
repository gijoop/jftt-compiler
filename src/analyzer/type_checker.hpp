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

    void visit(ProcHeadNode& node) override {
        for (const auto& arg : node.args_decl()->arguments()) {
            param_types_[scoped(arg.name)] = arg.type;
            vars_[scoped(arg.name)].type = arg.type;
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
    std::map<std::string, AST::ParamType> param_types_;
    std::map<std::string, VarInfo> vars_;
};

} // namespace AST