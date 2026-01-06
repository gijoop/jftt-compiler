#pragma once

#include <vector>
#include <string>

#include "ast/visitor.hpp"
#include "tac/tac.hpp"
#include "ast/ast.hpp"

class TacGenerator : public AstVisitor {
    Tac::Program& program;
    Tac::Operand last_result;

public:
    TacGenerator(Tac::Program& p) : program(p), last_result(Tac::Operand::make_const(0)) {}

    void visit(AST::IdentifierNode& node) override {
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

    void visit(AST::AssignmentNode& node) override {
        node.expr()->accept(*this);
        Tac::Operand target = Tac::Operand::make_var(node.id()->get_name());
        
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
        node.main()->accept(*this);
        program.emit({Tac::OpCode::HALT, std::nullopt, std::nullopt, std::nullopt});
    }

    // Przykład obsługi przyszłego IF (dla instrukcji warunkowych)
    /*
    void visit(IfNode& node) {
        Tac::Operand label_else = program.new_label();
        Tac::Operand label_end = program.new_label();

        node.condition()->accept(*this);
        // Zakładamy, że condition zwraca 0 dla fałszu
        program.emit({Tac::OpCode::JIFZ, last_result, std::nullopt, label_else});

        node.then_block()->accept(*this);
        program.emit({Tac::OpCode::JMP, label_end, std::nullopt, std::nullopt});

        program.emit({Tac::OpCode::LABEL, label_else, std::nullopt, std::nullopt});
        if(node.else_block()) node.else_block()->accept(*this);
        
        program.emit({Tac::OpCode::LABEL, label_end, std::nullopt, std::nullopt});
    }
    */
};