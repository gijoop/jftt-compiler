#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "ast/visitor.hpp"
#include "tac/tac.hpp"
#include "ast/ast.hpp"

class TacGenerator : public AstVisitor {
    Tac::Program& program;
    Tac::Operand last_result;
    
    std::string current_procedure;
    std::unordered_map<std::string, std::string> param_to_reference; // param_name -> reference_name

public:
    TacGenerator(Tac::Program& p) : program(p), last_result(Tac::Operand::make_const(0)) {}

    void visit(AST::IdentifierNode& node) override {
        if (!current_procedure.empty()) {
            auto it = param_to_reference.find(node.get_name());
            if (it != param_to_reference.end()) {
                last_result = Tac::Operand::make_reference(it->second);
                return;
            } else{
                last_result = Tac::Operand::make_var(current_procedure + "." + node.get_name());
                return;
            }
        }
        last_result = Tac::Operand::make_var(node.get_name());
    }

    void visit(AST::ConstantNode& node) override {
        last_result = Tac::Operand::make_const(node.value());
    }

    void visit(AST::WriteNode& node) override {
        node.expr()->accept(*this);
        program.emit({Tac::OpCode::WRITE, last_result, std::nullopt, std::nullopt});
    }

    void visit(AST::ReadNode& node) override {
        Tac::Operand target = Tac::Operand::make_var(node.id()->get_name());
        program.emit({Tac::OpCode::READ, std::nullopt, std::nullopt, target});
    }

    void visit(AST::BinaryOpNode& node) override {
        node.left()->accept(*this);
        Tac::Operand op_left = last_result;

        node.right()->accept(*this);
        Tac::Operand op_right = last_result;

        Tac::Operand op_res = program.new_temp();
        Tac::OpCode code = util::to_tac_op(node.op());

        program.emit({code, op_left, op_right, op_res});
        last_result = op_res;
    }

    void visit(AST::BinaryCondNode& node) override {
        node.left()->accept(*this);
        Tac::Operand op_left = last_result;

        node.right()->accept(*this);
        Tac::Operand op_right = last_result;

        Tac::Operand op_res = program.new_temp();
        Tac::OpCode code = util::to_tac_op(node.op());

        program.emit({code, op_left, op_right, op_res});
        last_result = op_res;
    }

    void visit(AST::AssignmentNode& node) override {
        node.expr()->accept(*this);
        Tac::Operand target;
        if (!current_procedure.empty()) {
            auto it = param_to_reference.find(node.id()->get_name());
            if (it != param_to_reference.end()) {
                target = Tac::Operand::make_reference(it->second);
            } else {
                target = Tac::Operand::make_var(current_procedure + "." + node.id()->get_name());
            }
        } else {
            target = Tac::Operand::make_var(node.id()->get_name());
        }
        
        program.emit({Tac::OpCode::ASSIGN, last_result, std::nullopt, target});
    }

    void visit(AST::CommandsNode& node) override {
        for (auto& cmd : node.commands()) {
            cmd->accept(*this);
        }
    }

    void visit(AST::MainNode& node) override {
        if (node.commands()) {
            node.commands()->accept(*this);
        }
    }

    void visit(AST::ProgramNode& node) override {
        auto main_label = Tac::Operand::make_label("main");
        program.emit({Tac::OpCode::JMP, main_label, std::nullopt, std::nullopt});

        node.procedures()->accept(*this);

        program.emit({Tac::OpCode::LABEL, main_label, std::nullopt, std::nullopt});

        node.main()->accept(*this);

        program.emit({Tac::OpCode::HALT, std::nullopt, std::nullopt, std::nullopt});
    }

    void visit(AST::IfNode& node) override {
        Tac::Operand label_else = program.new_label();
        Tac::Operand label_end = program.new_label();

        node.condition()->accept(*this);
        // IF condition is false, jump to ELSE
        program.emit({Tac::OpCode::JZERO, last_result, std::nullopt, label_else});

        node.then_commands()->accept(*this);
        program.emit({Tac::OpCode::JMP, label_end, std::nullopt, std::nullopt});

        program.emit({Tac::OpCode::LABEL, label_else, std::nullopt, std::nullopt});
        if(node.else_commands()) node.else_commands().value()->accept(*this);
        
        program.emit({Tac::OpCode::LABEL, label_end, std::nullopt, std::nullopt});
    }

    void visit(AST::WhileNode& node) override {
        Tac::Operand label_start = program.new_label();
        Tac::Operand label_end = program.new_label();

        program.emit({Tac::OpCode::LABEL, label_start, std::nullopt, std::nullopt});

        node.condition()->accept(*this);
        // If condition is false, exit loop
        program.emit({Tac::OpCode::JZERO, last_result, std::nullopt, label_end});

        node.commands()->accept(*this);
        program.emit({Tac::OpCode::JMP, label_start, std::nullopt, std::nullopt});

        program.emit({Tac::OpCode::LABEL, label_end, std::nullopt, std::nullopt});
    }

    void visit(AST::RepeatNode& node) override {
        Tac::Operand label_start = program.new_label();
        Tac::Operand label_end = program.new_label();

        program.emit({Tac::OpCode::LABEL, label_start, std::nullopt, std::nullopt});
        
        node.commands()->accept(*this);

        node.condition()->accept(*this);
        // If condition is true, exit loop
        program.emit({Tac::OpCode::JPOS, last_result, std::nullopt, label_end});

        program.emit({Tac::OpCode::JMP, label_start, std::nullopt, std::nullopt});

        program.emit({Tac::OpCode::LABEL, label_end, std::nullopt, std::nullopt});
    }

    void visit(AST::ProcHeadNode& node) override {}

    void visit(AST::ProcedureNode& node) override {
        // Enter procedure context
        current_procedure = node.head()->name();
        
        // Register parameters for this procedure
        std::vector<std::string> param_names;
        for (const auto& arg : node.head()->args_decl()->arguments()) {
            param_names.push_back(arg);
            std::string ref_name = current_procedure + ".arg." + std::to_string(param_names.size() - 1);
            param_to_reference[arg] = ref_name;
        }
        
        // Procedure label
        Tac::Operand proc_label = Tac::Operand::make_label(node.head()->name());
        program.emit({Tac::OpCode::LABEL, proc_label, std::nullopt, std::nullopt});

        auto tmp_proc_return = program.new_temp();
        program.emit({Tac::OpCode::FUNC_ENTER, tmp_proc_return, std::nullopt, std::nullopt});

        // Procedure body
        node.head()->accept(*this);
        node.commands()->accept(*this); 

        // End of procedure - return
        program.emit({Tac::OpCode::FUNC_EXIT, tmp_proc_return, std::nullopt, std::nullopt});
        program.emit({Tac::OpCode::RETURN, std::nullopt, std::nullopt, std::nullopt});
        
        // Exit procedure context and clean up parameter mappings
        for (const auto& param : param_names) {
            param_to_reference.erase(param);
        }
        current_procedure.clear();
    }

    void visit(AST::ProcedureCallNode& node) override {
        int i = 0;
        for (const auto& arg : node.args()->arguments()) {
            Tac::Operand arg_var;
            if (!current_procedure.empty()) {
                auto it = param_to_reference.find(arg);
                if (it != param_to_reference.end()) {
                    arg_var = Tac::Operand::make_reference(it->second);
                } else {
                    arg_var = Tac::Operand::make_var(current_procedure + "." + arg);
                }
            } else {
                arg_var = Tac::Operand::make_var(arg);
            }
            auto arg_reference = Tac::Operand::make_reference(node.name() + ".arg." + std::to_string(i++));
            program.emit({Tac::OpCode::PARAM, arg_var, arg_reference, std::nullopt});
        }
        
        // Call procedure
        Tac::Operand proc_label = Tac::Operand::make_label(node.name());
        program.emit({Tac::OpCode::CALL, proc_label, std::nullopt, std::nullopt});
    }
};
