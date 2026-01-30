#pragma once

#include "ast/ast.hpp"
#include "tac/tac_generator.hpp"
#include "asm/asm_generator.hpp"
#include "parser/parser_wrapper.hpp"
#include "analyzer/declaration_checker.hpp"
#include "analyzer/procedure_checker.hpp"
#include "analyzer/type_checker.hpp"
#include "optimizer/optimizer.hpp"
#include "optimizer/tac_optimizer.hpp"
#include "optimizer/asm_optimizer.hpp"


class Compiler {
public:
    Compiler() = default;

    void set_optimization(bool enable) {
        optimize_ = enable;
    }

    std::string compile(const std::string& source_code) {
        ParserWrapper parser;
        if (!parser.parse(source_code)) {
            throw std::runtime_error("Parsing failed.");
        }

        auto ast = parser.get_result();
        if (!ast) {
            throw std::runtime_error("No AST generated.");
        }

        AST::DeclarationChecker decl_checker;
        ast->accept(decl_checker);

        AST::ProcedureChecker proc_checker;
        ast->accept(proc_checker);

        AST::TypeChecker type_checker;
        ast->accept(type_checker);

        Tac::Program tac_program;
        {
            TacGenerator tac_gen(tac_program);
            ast->accept(tac_gen);
        }

        if (optimize_) {
            auto tac_optimizer = Optimizer::create_default_tac_optimizer();
            tac_optimizer.optimize(tac_program);
        }

        AsmGenerator asm_generator(tac_program);
        auto asm_code = asm_generator.compile();

        if (optimize_) {
            auto asm_optimizer = Optimizer::create_default_asm_optimizer();
            asm_optimizer.optimize(asm_code);
            
            reassign_addresses(asm_code);
        }

        std::string result;
        for (const auto& instr : asm_code) {
            if (instr.op == Asm::Code::LABEL) continue;
            result += instr.to_string() + "\n";
        }

        return result;
    }

private:
    bool optimize_ = true;
    
    void reassign_addresses(std::vector<Asm::Instruction>& code) {
        long long address_counter = 0;
        for (auto& instr : code) {
            if (instr.op == Asm::Code::LABEL) {
                instr.label->address = address_counter;
            } else {
                address_counter++;
            }
        }
    }
};