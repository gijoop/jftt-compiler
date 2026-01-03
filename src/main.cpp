#include <iostream>
#include <fstream>
#include <string>

#include "ast/lang.hpp"
#include "ast/tac.hpp"
#include "parser_wrapper.hpp"

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

    auto lang_ast_base = parser.get_result();
    if (!lang_ast_base) {
        std::cerr << "Error: No AST generated." << std::endl;
        return 1;
    }

    auto* lang_program = dynamic_cast<LangAST::ProgramNode*>(lang_ast_base.get());
    if (!lang_program) {
        std::cerr << "Error: Root node is not a ProgramNode." << std::endl;
        return 1;
    }

    auto tac_program = lang_program->to_tac_program();
    std::cout << "TAC AST:\n" << tac_program->to_string() << std::endl;
    auto asm_code = tac_program->to_asm();

    std::string output_file = argv[2];
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return 1;
    }

    for (const auto& instr : asm_code) {
        out << instr->to_string() << "\n";
    }

    out.close();
    std::cout << "Compilation successful! Saved to: " << output_file << std::endl;

    return 0;
}