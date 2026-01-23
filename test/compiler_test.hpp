#pragma once

#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <string>

#include "compiler.hpp"

class CompilerTest : public testing::Test {
    protected:
    
    void SetUp() override {
        SymbolTable::reset();
    }
    
    void TearDown() override {
        SymbolTable::reset();
    }
    
    std::string run_vm(const std::string& asm_code, const std::vector<std::string>& input_data = {}) {
        std::string asm_filename = "/tmp/temp_code.asm";
        std::string input_filename = "/tmp/temp_input.txt";
        
        {
            std::ofstream asm_file(asm_filename);
            asm_file << asm_code;
        }
        
        {
            std::ofstream input_file(input_filename);
            for (const auto& line : input_data) {
                input_file << line << "\n";
            }
        }
        
        std::array<char, 128> buffer;
        std::string result;
        
        std::string cmd = "/home/lucas/jftt-compiler/mw2025/maszyna-wirtualna " + asm_filename + " < " + input_filename;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        
        std::remove(asm_filename.c_str());
        std::remove(input_filename.c_str());
        
        return result;
    }
    
    std::vector<std::string> compile_and_run(const std::string& source_code, const std::vector<std::string>& input_data = {}) {
        Compiler compiler;
        std::string asm_code = compiler.compile(source_code);
        
        std::string full_output = run_vm(asm_code, input_data);
        
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