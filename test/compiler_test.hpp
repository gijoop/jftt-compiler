#pragma once

#include "gtest/gtest.h"
#include "compiler.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <regex>
#include <sys/wait.h>

class CompilerTest : public testing::Test {
protected:
    static inline long long total_cost_raw_ = 0;
    static inline long long total_cost_opt_ = 0;
    static inline int test_count_ = 0;

    static void SetUpTestSuite() {
        total_cost_raw_ = 0;
        total_cost_opt_ = 0;
        test_count_ = 0;
    }

    static void TearDownTestSuite() {
        if (test_count_ > 0) {
            double avg_raw = static_cast<double>(total_cost_raw_) / test_count_;
            double avg_opt = static_cast<double>(total_cost_opt_) / test_count_;
            double improvement = (1.0 - (avg_opt / avg_raw)) * 100.0;

            std::cout << "\n";
            std::cout << "==================================================================================\n";
            std::cout << "SUMMARY (" << test_count_ << " tests)\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Avg Cost (Raw): " << avg_raw << "\n";
            std::cout << "  Avg Cost (Opt): " << avg_opt << " (" << (improvement > 0 ? "+" : "") << improvement << "% improvement)\n";
            std::cout << "==================================================================================\n";
        }
    }

    void SetUp() override { SymbolTable::reset(); }
    void TearDown() override { SymbolTable::reset(); }

    std::string strip_ansi(const std::string& input) {
        return std::regex_replace(input, std::regex("\033\\[[0-9;]*m"), "");
    }

    long long parse_cost(const std::string& vm_output) {
        std::istringstream iss(vm_output);
        std::string line;
        while (std::getline(iss, line)) {
            std::string clean = strip_ansi(line);
            if (auto pos = clean.find("koszt: "); pos != std::string::npos) {
                std::string part = clean.substr(pos + 7, clean.find(';', pos) - (pos + 7));
                std::string num;
                for (char c : part) if (isdigit(c)) num += c;
                return num.empty() ? 0 : std::stoll(num);
            }
        }
        return 0;
    }

    std::string run_vm(const std::string& asm_code, const std::vector<std::string>& input_data) {
        std::string asm_file = "/tmp/temp_code.asm";
        std::string inp_file = "/tmp/temp_input.txt";

        { std::ofstream f(asm_file); f << asm_code; }
        { std::ofstream f(inp_file); for (const auto& l : input_data) f << l << "\n"; }

        std::string cmd = "timeout 1 /home/lucas/jftt-compiler/mw2025/maszyna-wirtualna " + asm_file + " < " + inp_file + " 2>&1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) throw std::runtime_error("popen failed");

        std::array<char, 128> buffer;
        std::string result;
        while (fgets(buffer.data(), buffer.size(), pipe)) result += buffer.data();

        int status = pclose(pipe);
        std::remove(asm_file.c_str());
        std::remove(inp_file.c_str());
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 124) {
            throw std::runtime_error("VM execution exceeded 1 second timeout");
        }
        
        return result;
    }

    std::vector<std::string> compile_and_run(const std::string& source_code, const std::vector<std::string>& input_data = {}) {
        Compiler c_raw;
        c_raw.set_optimization(false);
        std::string asm_raw = c_raw.compile(source_code);

        // Check if assembly contains negative addresses
        std::istringstream asm_stream(asm_raw);
        std::string asm_line;
        while (std::getline(asm_stream, asm_line)) {
            std::string clean = strip_ansi(asm_line);
            if (clean.find("-") != std::string::npos) {
                throw std::runtime_error("Generated assembly contains negative addresses");
            }
        }

        std::string out_raw = run_vm(asm_raw, input_data);
        long long cost_raw = parse_cost(out_raw);
        int lines_raw = std::count(asm_raw.begin(), asm_raw.end(), '\n');

        Compiler c_opt;
        c_opt.set_optimization(true);
        std::string asm_opt = c_opt.compile(source_code);
        std::string out_opt = run_vm(asm_opt, input_data);
        long long cost_opt = parse_cost(out_opt);
        int lines_opt = std::count(asm_opt.begin(), asm_opt.end(), '\n');

        total_cost_raw_ += cost_raw;
        total_cost_opt_ += cost_opt;
        test_count_++;

        RecordProperty("cost_raw", std::to_string(cost_raw));
        RecordProperty("lines_raw", std::to_string(lines_raw));
        RecordProperty("cost_opt", std::to_string(cost_opt));
        RecordProperty("lines_opt", std::to_string(lines_opt));

        std::vector<std::string> user_output;
        std::istringstream iss(out_opt);
        std::string line;
        while (std::getline(iss, line)) {
            std::string clean = strip_ansi(line);
            if (auto pos = clean.find("> "); pos != std::string::npos) {
                user_output.push_back(clean.substr(pos + 2));
            }
        }
        return user_output;
    }
};