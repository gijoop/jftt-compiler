#pragma once

#include "ast/ast.hpp"
#include "tac/tac_generator.hpp"
#include "asm/asm_generator.hpp"
#include "parser/parser_wrapper.hpp"
#include "analyzer/declaration_checker.hpp"
#include "analyzer/procedure_checker.hpp"
#include "analyzer/type_checker.hpp"


class Compiler {
public:
    Compiler() = default;

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

        AsmGenerator asm_generator(tac_program);
        auto asm_code = asm_generator.compile();

        std::string result;
        for (const auto& instr : asm_code) {
            result += instr.to_string() + "\n";
        }

        return result;
    }
};