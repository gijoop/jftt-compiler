#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ast/visitor.hpp"
#include "tac/tac.hpp"
#include "ast/ast.hpp"
#include "symbol_table.hpp"

class TacGenerator : public AstVisitor {
    Tac::Program& program;
    Tac::Operand last_result;
    
    std::string current_procedure;
    std::unordered_map<std::string, std::string> param_to_reference;
    std::unordered_set<std::string> array_params;

    std::string scoped(const std::string& name) const {
        return current_procedure.empty() ? name : current_procedure + "." + name;
    }

    Tac::Operand resolve_var(const std::string& name) {
        auto it = param_to_reference.find(name);
        if (it != param_to_reference.end()) {
            return Tac::Operand::make_reference(it->second);
        }
        return Tac::Operand::make_var(scoped(name));
    }

    Tac::Operand resolve_array(const std::string& name) {
        if (array_params.count(name)) {
            return Tac::Operand::make_reference(param_to_reference.at(name));
        }
        return Tac::Operand::make_array(scoped(name), 0);
    }

    Tac::Operand get_index_operand(const AST::IdentifierNode& node) {
        if (node.get_index_name()) {
            return resolve_var(*node.get_index_name());
        }
        return Tac::Operand::make_const(*node.get_index_value());
    }

    Tac::Operand resolve_arg(const std::string& name) {
        auto it = param_to_reference.find(name);
        if (it != param_to_reference.end()) {
            return Tac::Operand::make_reference(it->second);
        }
        
        std::string scoped_name = scoped(name);
        if (SymbolTable::is_array(scoped_name)) {
            return Tac::Operand::make_array(scoped_name, 0);
        }
        return Tac::Operand::make_var(scoped_name);
    }

    void emit(Tac::OpCode op, 
              std::optional<Tac::Operand> a1 = std::nullopt,
              std::optional<Tac::Operand> a2 = std::nullopt,
              std::optional<Tac::Operand> res = std::nullopt) {
        program.emit({op, a1, a2, res});
    }

public:
    TacGenerator(Tac::Program& p) : program(p), last_result(Tac::Operand::make_const(0)) {}

    void visit(AST::IdentifierNode& node) override {
        if (node.get_type() == AST::IdType::ARRAY) {
            auto arr_op = resolve_array(node.get_name());
            auto idx_op = get_index_operand(node);
            
            auto result = program.new_temp();
            emit(Tac::OpCode::ARRAY_LOAD, arr_op, idx_op, result);
            last_result = result;
        } else {
            last_result = resolve_var(node.get_name());
        }
    }

    void visit(AST::ConstantNode& node) override {
        last_result = Tac::Operand::make_const(node.value());
    }

    void visit(AST::WriteNode& node) override {
        node.expr()->accept(*this);
        emit(Tac::OpCode::WRITE, last_result);
    }

    void visit(AST::ReadNode& node) override {
        auto& id = node.id();
        
        if (id->get_type() == AST::IdType::ARRAY) {
            auto arr_op = resolve_array(id->get_name());
            auto idx_op = get_index_operand(*id);
            auto temp = program.new_temp();
            emit(Tac::OpCode::READ, std::nullopt, std::nullopt, temp);
            emit(Tac::OpCode::ARRAY_STORE, arr_op, idx_op, temp);
        } else {
            auto target = resolve_var(id->get_name());
            emit(Tac::OpCode::READ, std::nullopt, std::nullopt, target);
        }
    }

    void visit(AST::BinaryOpNode& node) override {
        node.left()->accept(*this);
        auto left = last_result;
        node.right()->accept(*this);
        auto right = last_result;

        auto result = program.new_temp();
        emit(util::to_tac_op(node.op()), left, right, result);
        last_result = result;
    }

    void visit(AST::BinaryCondNode& node) override {
        node.left()->accept(*this);
        auto left = last_result;
        node.right()->accept(*this);
        auto right = last_result;

        auto result = program.new_temp();
        emit(util::to_tac_op(node.op()), left, right, result);
        last_result = result;
    }

    void visit(AST::AssignmentNode& node) override {
        node.expr()->accept(*this);
        auto value = last_result;
        auto& id = node.id();
        
        if (id->get_type() == AST::IdType::ARRAY) {
            auto arr_op = resolve_array(id->get_name());
            auto idx_op = get_index_operand(*id);
            emit(Tac::OpCode::ARRAY_STORE, arr_op, idx_op, value);
        } else {
            auto target = resolve_var(id->get_name());
            emit(Tac::OpCode::ASSIGN, value, std::nullopt, target);
        }
    }

    void visit(AST::DeclarationsNode& node) override {
        for (auto& decl : node.declarations()) {
            std::string name = scoped(decl.name);
            if (decl.is_array) {
                SymbolTable::add_array(name, decl.start_index, decl.end_index);
            } else {
                SymbolTable::add_symbol(name);
            }
        }
    }

    void visit(AST::ProgramNode& node) override {
        auto main_label = Tac::Operand::make_label("main");
        emit(Tac::OpCode::JMP, main_label);

        node.procedures()->accept(*this);

        emit(Tac::OpCode::LABEL, main_label);
        node.main()->accept(*this);
        emit(Tac::OpCode::HALT);
    }

    void visit(AST::MainNode& node) override {
        if (node.declarations()) (*node.declarations())->accept(*this);
        if (node.commands()) node.commands()->accept(*this);
    }

    void visit(AST::IfNode& node) override {
        auto label_else = program.new_label();
        auto label_end = program.new_label();

        node.condition()->accept(*this);
        emit(Tac::OpCode::JZERO, last_result, std::nullopt, label_else);

        node.then_commands()->accept(*this);
        emit(Tac::OpCode::JMP, label_end);

        emit(Tac::OpCode::LABEL, label_else);
        if (node.else_commands()) node.else_commands().value()->accept(*this);
        
        emit(Tac::OpCode::LABEL, label_end);
    }

    void visit(AST::WhileNode& node) override {
        auto label_start = program.new_label();
        auto label_end = program.new_label();

        emit(Tac::OpCode::LABEL, label_start);
        node.condition()->accept(*this);
        emit(Tac::OpCode::JZERO, last_result, std::nullopt, label_end);

        node.commands()->accept(*this);
        emit(Tac::OpCode::JMP, label_start);

        emit(Tac::OpCode::LABEL, label_end);
    }

    void visit(AST::RepeatNode& node) override {
        auto label_start = program.new_label();
        auto label_end = program.new_label();

        emit(Tac::OpCode::LABEL, label_start);
        node.commands()->accept(*this);
        node.condition()->accept(*this);
        emit(Tac::OpCode::JPOS, last_result, std::nullopt, label_end);
        emit(Tac::OpCode::JMP, label_start);
        emit(Tac::OpCode::LABEL, label_end);
    }

    void visit(AST::ProcedureNode& node) override {
        current_procedure = node.head()->name();
        
        int idx = 0;
        for (const auto& arg : node.head()->args_decl()->arguments()) {
            std::string ref_name = current_procedure + ".arg." + std::to_string(idx++);
            param_to_reference[arg] = ref_name;
            array_params.insert(arg);
        }
        
        emit(Tac::OpCode::LABEL, Tac::Operand::make_label(current_procedure));
        
        auto return_addr = program.new_temp();
        emit(Tac::OpCode::FUNC_ENTER, return_addr);

        if (node.declarations()) (*node.declarations())->accept(*this);
        node.commands()->accept(*this);

        emit(Tac::OpCode::FUNC_EXIT, return_addr);
        emit(Tac::OpCode::RETURN);
        
        for (const auto& arg : node.head()->args_decl()->arguments()) {
            param_to_reference.erase(arg);
            array_params.erase(arg);
        }
        current_procedure.clear();
    }

    void visit(AST::ProcedureCallNode& node) override {
        int idx = 0;
        for (const auto& arg : node.args()->arguments()) {
            auto arg_op = resolve_arg(arg);
            auto param_ref = Tac::Operand::make_reference(node.name() + ".arg." + std::to_string(idx++));
            
            auto opcode = (arg_op.type == Tac::OperandType::ARRAY) 
                        ? Tac::OpCode::ARRAY_PARAM 
                        : Tac::OpCode::PARAM;
            emit(opcode, arg_op, param_ref);
        }
        
        emit(Tac::OpCode::CALL, Tac::Operand::make_label(node.name()));
    }
};
