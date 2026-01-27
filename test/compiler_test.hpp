#pragma once

#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <string>

#include "compiler.hpp"

class CompilerTest : public testing::Test {
protected:

    static inline int total_program_cost_ = 0;
    static inline int total_instruction_count_ = 0;

    static inline int max_program_cost_ = 0;
    static inline int max_instruction_count_ = 0;

    static inline int test_count_ = 0;

    static void SetUpTestSuite() {
        total_program_cost_ = 0;
        total_instruction_count_ = 0;
        max_program_cost_ = 0;
        max_instruction_count_ = 0;
        test_count_ = 0;
    }

    static void TearDownTestSuite() {
        double avg_cost = total_program_cost_ / static_cast<double>(test_count_);
        double avg_instructions = total_instruction_count_ / static_cast<double>(test_count_);

        std::cout << "Average Program Cost: " << avg_cost << std::endl;
        std::cout << "Average Instruction Count: " << avg_instructions << std::endl;
    }

    
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
    
    std::vector<std::string> compile_and_run(const std::string& source_code, const std::vector<std::string>& input_data = {}, bool optimize = false) {
        Compiler compiler;
        compiler.set_optimization(optimize);
        std::string asm_code = compiler.compile(source_code);
        
        std::string full_output = run_vm(asm_code, input_data);
        
        std::vector<std::string> output_lines;
        std::istringstream iss(full_output);
        std::string line;
        while (std::getline(iss, line)) {
            output_lines.push_back(line);
        }
        
        std::vector<std::string> write_outputs;
        int program_cost = 0;
        for (auto& line : output_lines) {
            if (line.find("> ") != std::string::npos) {
                size_t start = line.find("> ") + 2;
                write_outputs.push_back(line.substr(start));
            }
            else if (line.find("koszt: \033[31m") != std::string::npos) {
                size_t start = line.find("koszt: \033[31m") + 13;
                size_t end = line.find("\033[0m;", start);
                std::string cost_str = line.substr(start, end - start);
                cost_str.erase(remove_if(cost_str.begin(), cost_str.end(), isspace), cost_str.end());
                program_cost = std::stoi(cost_str);
            }
        }

        total_program_cost_ += program_cost;
        max_program_cost_ = std::max(max_program_cost_, program_cost);

        int instruction_count = std::count(asm_code.begin(), asm_code.end(), '\n');
        max_instruction_count_ = std::max(max_instruction_count_, instruction_count);
        total_instruction_count_ += instruction_count;

        test_count_++;

        return write_outputs;
    }
};