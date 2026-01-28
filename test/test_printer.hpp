#pragma once
#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>
#include <cstring>

namespace Color {
    const char* RESET   = "\033[0m";
    const char* RED     = "\033[31m";
    const char* GREEN   = "\033[32m";
    const char* GRAY    = "\033[90m";
    const char* CYAN    = "\033[36m";
}

class CustomPrinter : public testing::EmptyTestEventListener {
public:
    void OnTestProgramStart(const testing::UnitTest& /*unit_test*/) override {
        std::cout << "\n";
        std::cout << std::left 
                  << std::setw(35) << "TEST NAME" 
                  << std::setw(10) << "STATUS" 
                  << std::setw(12) << "COST(RAW)" 
                  << std::setw(12) << "LINES(RAW)" 
                  << std::setw(12) << "COST(OPT)" 
                  << std::setw(12) << "LINES(OPT)" << "\n";
        std::cout << "--------------------------------------------------------------------------------------------------\n";
    }

    void OnTestEnd(const testing::TestInfo& test_info) override {
        bool passed = test_info.result()->Passed();
        const char* status_color = passed ? Color::GREEN : Color::RED;
        std::string status_text = passed ? "PASS" : "FAIL";

        std::string c_raw = "-", l_raw = "-", c_opt = "-", l_opt = "-";

        const auto* result = test_info.result();
        int count = result->test_property_count();
        for (int i = 0; i < count; ++i) {
            const auto& prop = result->GetTestProperty(i);
            if (strcmp(prop.key(), "cost_raw") == 0) c_raw = prop.value();
            if (strcmp(prop.key(), "lines_raw") == 0) l_raw = prop.value();
            if (strcmp(prop.key(), "cost_opt") == 0) c_opt = prop.value();
            if (strcmp(prop.key(), "lines_opt") == 0) l_opt = prop.value();
        }

        std::cout << std::left 
                  << std::setw(45) << test_info.name() 
                  << status_color << std::setw(10) << status_text << Color::RESET
                  << std::setw(12) << c_raw 
                  << std::setw(12) << l_raw 
                  << std::setw(12) << c_opt
                  << std::setw(12) << l_opt 
                  << std::endl;

        if (!passed) {
            for (int i = 0; i < result->total_part_count(); ++i) {
                const auto& part = result->GetTestPartResult(i);
                if (part.failed()) {
                    std::cout << Color::GRAY << "   " << part.summary() 
                              << " (" << part.file_name() << ":" << part.line_number() << ")" 
                              << Color::RESET << "\n";
                }
            }
        }
    }

    void OnTestProgramEnd(const testing::UnitTest& unit_test) override {
        std::cout << "--------------------------------------------------------------------------------------------------\n";
        std::cout << "Total: " << unit_test.total_test_count()
                  << " | Passed: " << Color::GREEN << unit_test.successful_test_count() << Color::RESET
                  << " | Failed: " << Color::RED << unit_test.failed_test_count() << Color::RESET << "\n";
    }
};