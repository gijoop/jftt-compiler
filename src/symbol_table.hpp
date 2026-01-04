#pragma once
#include <map>
#include <string>

class SymbolTable {
public:
    static long long get_address(const std::string& name) {
        if (!is_declared(name)) {
            declare(name);
        }
        return instance().table_[name];
    }

    static std::string get_temp_var_name() {
        std::string var_name = "_temp." + std::to_string(instance().temp_var_count_++);
        if (!is_declared(var_name)) {
            declare(var_name);
        }
        return var_name;
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
        instance().temp_var_count_ = 0;
        instance().instruction_counter_ = 0;
    }

    static void inc_ic() {
        instance().instruction_counter_++;
    }

    static long long get_ic() {
        return instance().instruction_counter_;
    }

private:
    SymbolTable() : next_addr_(0), temp_var_count_(0), instruction_counter_(0) {}
    
    static SymbolTable& instance() {
        static SymbolTable inst;
        return inst;
    }

    std::map<std::string, long long> table_;
    long long next_addr_;
    long long temp_var_count_;
    long long instruction_counter_;
};