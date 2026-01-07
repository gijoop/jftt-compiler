#include <iostream>
#include <fstream>
#include <string>

#include "ast/ast.hpp"
#include "tac/tac_generator.hpp"
#include "asm/asm_generator.hpp"
#include "parser/parser_wrapper.hpp"
#include "analyzer/declaration_checker.hpp"

#define USAGE "Usage: " << argv[0] << " <source_code> <output_file>"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << USAGE << std::endl;
        return 1;
    }

    std::string source_code_file = argv[1];
    std::ifstream file(source_code_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << source_code_file << std::endl;
        return 1;
    }
    std::string source_code((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();

    ParserWrapper parser;
    if (!parser.parse(source_code)) {
        return 1;
    }

    auto lang_ast = parser.get_result();
    if (!lang_ast) {
        std::cerr << "Error: No AST generated." << std::endl;
        return 1;
    }

    try {
        AST::DeclarationChecker decl_checker;
        lang_ast->accept(decl_checker);
    } catch (const SemanticError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << "Lang AST:\n" << lang_ast->to_string() << std::endl;


    Tac::Program tac_program;
    {
        TacGenerator tac_gen(tac_program);
        lang_ast->accept(tac_gen);
    }

    std::cout << "Generated TAC Instructions:\n";
    for (const auto& instr : tac_program.instructions) {
        std::cout << instr.to_string() << std::endl;
    }
    AsmGenerator compiler(tac_program);
    auto asm_code = compiler.compile();

    std::string output_file = argv[2];
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return 1;
    }
    for (const auto& instr : asm_code) {
        out << instr.to_string() << std::endl;
    }
    out.close();
    std::cout << "Compilation successful! Saved to: " << output_file << std::endl;

    return 0;
}