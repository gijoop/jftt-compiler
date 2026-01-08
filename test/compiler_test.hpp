#pragma once

#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <string>

#include "ast/ast.hpp"
#include "tac/tac_generator.hpp"
#include "asm/asm_generator.hpp"
#include "parser/parser_wrapper.hpp"
#include "analyzer/declaration_checker.hpp"
#include "symbol_table.hpp"

class CompilerTest : public testing::Test {
protected:

  void SetUp() override {
    SymbolTable::reset();
  }

  void TearDown() override {
    SymbolTable::reset();
  }

  std::string run_vm(const std::string& asm_code) {
    std::string asm_filename = "/tmp/temp_code.asm";
    {
      std::ofstream asm_file(asm_filename);
      asm_file << asm_code;
    }   

    std::array<char, 128> buffer;
    std::string result;
    
    std::string cmd = "./mw2025/maszyna-wirtualna " + asm_filename;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    
    if (!pipe) {
      throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }

    std::remove(asm_filename.c_str());
    
    return result;
  }

  std::vector<std::string> compile_and_run(const std::string& source_code) {
    ParserWrapper parser;
    if (!parser.parse(source_code)) {
      throw std::runtime_error("Parsing failed");
    }

    auto ast = parser.get_result();
    if (!ast) {
      throw std::runtime_error("No AST generated");
    }

    AST::DeclarationChecker decl_checker;
    ast->accept(decl_checker);

    Tac::Program tac_program;
    {
      TacGenerator tac_gen(tac_program);
      ast->accept(tac_gen);
    }

    AsmGenerator asm_gen(tac_program);
    auto asm_code = asm_gen.compile();

    std::string asm_code_str;
    for (const auto& instr : asm_code) {
      asm_code_str += instr.to_string() + "\n";
    }

    std::string full_output = run_vm(asm_code_str);

    std::vector<std::string> output_lines;
    std::istringstream iss(full_output);
    std::string line;
    while (std::getline(iss, line)) {
        output_lines.push_back(line);
    }

    std::vector<std::string> write_outputs; 
    for (auto& line : output_lines) {
      if (line.find("> ") != std::string::npos) {
        size_t start = line.find("> ") + 2;
        write_outputs.push_back(line.substr(start));
      }
    }

    return write_outputs;
  }
};