#pragma once
#include <map>
#include <string>

class SymbolTable {
public:
    static long long get_address(const std::string& name) {
        auto it = instance().table_.find(name);
        if (it == instance().table_.end()) {
            long long new_addr = instance().next_addr_++;
            instance().table_[name] = new_addr;
            return new_addr;
        }
        return it->second;
    }

    static std::string get_temp_var_name() {
        return "_tmp." + std::to_string(instance().temp_var_count_++);
    }

    static void reset() {
        instance().table_.clear();
        instance().next_addr_ = 0;
        instance().temp_var_count_ = 0;
    }

private:
    SymbolTable() : next_addr_(0), temp_var_count_(0) {}
    
    static SymbolTable& instance() {
        static SymbolTable inst;
        return inst;
    }

    std::map<std::string, long long> table_;
    long long next_addr_;
    long long temp_var_count_;
};