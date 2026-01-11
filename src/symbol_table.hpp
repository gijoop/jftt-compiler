#pragma once

#include <map>
#include <string>
#include <unordered_set>
#include <algorithm>

#include "ast/ast.hpp"
#include "tac/tac.hpp"

struct ArrayInfo {
    long long base_address;
    long long start_index;
    long long end_index;
};

class SymbolTable {
public:
    static long long get_address(Tac::Operand op) {
        std::string name;
        if (op.type == Tac::OperandType::VARIABLE || op.type == Tac::OperandType::REFERENCE) {
            name = op.name;
        } else if (op.type == Tac::OperandType::TEMP) {
            name = "tmp." + std::to_string(op.temp_id);
        } else if (op.type == Tac::OperandType::ARRAY) {
            name = op.name;
        }

        declare(name);
        return instance().table_[name];
    }

    static void reset() {
        instance().table_.clear();
        instance().arrays_.clear();
        instance().next_addr_ = 0;
    }

    static std::string dump() {
        std::string result;
        for (const auto& [name, addr] : instance().table_) {
            result += std::to_string(addr) + " -> " + name;
            auto it = instance().arrays_.find(name);
            if (it != instance().arrays_.end()) {
                result += " [array: " + std::to_string(it->second.start_index) + 
                          ".." + std::to_string(it->second.end_index) + "]";
            }
            result += "\n";
        }
        return result;
    }

    static void add_symbol(std::string name, long long size = 1) {
        instance().declare(name, size);
    }

    static void add_array(const std::string& name, long long start_index, long long end_index) {
        long long size = end_index - start_index + 1;
        instance().declare(name, size);
        instance().arrays_[name] = {instance().table_[name], start_index, end_index};
    }

    static bool is_array(const std::string& name) {
        return instance().arrays_.find(name) != instance().arrays_.end();
    }

    static ArrayInfo get_array_info(const std::string& name) {
        auto it = instance().arrays_.find(name);
        if (it != instance().arrays_.end()) {
            return it->second;
        }
        return {0, 0, 0};
    }

    static long long get_array_start_index(const std::string& name) {
        auto it = instance().arrays_.find(name);
        if (it != instance().arrays_.end()) {
            return it->second.start_index;
        }
        return 0;
    }

private:
    SymbolTable() : next_addr_(1), table_(), arrays_() {}
    
    static SymbolTable& instance() {
        static SymbolTable inst;
        return inst;
    }

    static bool declare(const std::string& name, long long size = 1) {
        if (is_declared(name)) {
            return false;
        }
        instance().table_[name] = instance().next_addr_;
        instance().next_addr_ += size;
        return true;
    }

    static bool is_declared(const std::string& name) {
        return instance().table_.find(name) != instance().table_.end();
    }

    std::map<std::string, long long> table_;
    std::map<std::string, ArrayInfo> arrays_;
    long long next_addr_;
};