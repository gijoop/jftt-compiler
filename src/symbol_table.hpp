#pragma once

#include <map>
#include <string>
#include <set>
#include <algorithm>

#include "ast/ast.hpp"
#include "tac/tac.hpp"

class SymbolTable {
public:
    static long long get_address(Tac::Operand op) {
        std::string name;
        if (op.type == Tac::OperandType::VARIABLE) {
            name = op.name;
        } else if (op.type == Tac::OperandType::TEMP) {
            name = "tmp." + std::to_string(op.temp_id);
        }

        declare(name);
        return instance().table_[name];
    }
    
    static bool declare(const std::string& name) {
        if (is_declared(name)) {
            return false;
        }
        instance().table_[name] = instance().next_addr_++;
        return true;
    }

    static bool is_declared(const std::string& name) {
        return instance().table_.find(name) != instance().table_.end();
    }

    static void reset() {
        instance().table_.clear();
        instance().next_addr_ = 0;
    }

private:
    SymbolTable() : next_addr_(0), table_() {}
    
    static SymbolTable& instance() {
        static SymbolTable inst;
        return inst;
    }

    std::map<std::string, long long> table_;
    long long next_addr_;
};