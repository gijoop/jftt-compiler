#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "compiler.hpp"

#define USAGE "Usage: " << argv[0] << " <source_code> <output_file> [-O0]"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << USAGE << std::endl;
        return 1;
    }

    std::string source_file = argv[1];
    std::string output_file = argv[2];
    bool optimize = true;
    
    // Check for -O0 flag to disable optimization
    for (int i = 3; i < argc; ++i) {
        if (std::strcmp(argv[i], "-O0") == 0) {
            optimize = false;
        }
    }

    std::ifstream file(source_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << source_file << std::endl;
        return 1;
    }

    std::string source_code((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();

    Compiler compiler;
    compiler.set_optimization(optimize);
    std::string asm_code;
    try {
        asm_code = compiler.compile(source_code);
    } catch (const SemanticError& e) {
        std::cerr << "Semantic error: " << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Internal error: " << e.what() << std::endl;
        return 1;
    }

    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return 1;
    }

    out << asm_code;
    out.close();

    
    std::cout << "Compilation successful! Saved to: " << output_file << std::endl;

    return 0;
}