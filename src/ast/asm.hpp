#pragma once

#include <string>
#include <vector>
#include <memory>

#include "register.hpp"
#include "util.hpp"

namespace AsmAST {

class Node {
public:
    virtual ~Node() = default;

    virtual std::string to_string(int level = 0) const {
        return util::pad(level) + to_instruction();
    }

    virtual std::string to_instruction() const = 0;
};

class ReadNode : public Node {
public:
    std::string to_instruction() const override {
        return "READ";
    }
};

class WriteNode : public Node {
public:
    std::string to_instruction() const override {
        return "WRITE";
    }
};

class LoadNode : public Node {
public:
    LoadNode(long long address) : address_(address) {}

    std::string to_instruction() const override {
        return "LOAD " + std::to_string(address_);
    }
private:
    long long address_;
};

class StoreNode : public Node {
public:
    StoreNode(long long address) : address_(address) {}

    std::string to_instruction() const override {
        return "STORE " + std::to_string(address_);
    }
private:
    long long address_;
};

class RLoad : public Node {
public:
    RLoad(Register reg) : reg_(std::move(reg)) {}
    std::string to_instruction() const override {
        return "RLOAD " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class RStore : public Node {
public:
    RStore(Register reg) : reg_(std::move(reg)) {}
    std::string to_instruction() const override {
        return "RSTORE " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class AddNode : public Node {
public:
    AddNode(Register reg) : reg_(std::move(reg)) {}

    std::string to_instruction() const override {
        return "ADD " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class SubtractNode : public Node {
public:
    SubtractNode(Register reg) : reg_(std::move(reg)) {}
    
    std::string to_instruction() const override {
        return "SUB " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class SwapNode : public Node {
public:
    SwapNode(Register reg) : reg_(std::move(reg)) {}

    std::string to_instruction() const override {
        return "SWP " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class ResetNode : public Node {
public:
    ResetNode(Register reg) : reg_(std::move(reg)) {}

    std::string to_instruction() const override {
        return "RST " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class IncrementNode : public Node {
public:
    IncrementNode(Register reg) : reg_(std::move(reg)) {}

    std::string to_instruction() const override {
        return "INC " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class DecrementNode : public Node {
public:
    DecrementNode(Register reg) : reg_(std::move(reg)) {}

    std::string to_instruction() const override {
        return "DEC " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class ShiftLeftNode : public Node {
public:
    ShiftLeftNode(Register reg) : reg_(std::move(reg)) {}
    std::string to_instruction() const override {
        return "SHL " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class ShiftRightNode : public Node {
public:
    ShiftRightNode(Register reg) : reg_(std::move(reg)) {}

    std::string to_instruction() const override {
        return "SHR " + util::to_string(reg_);
    }
private:
    Register reg_;
};

class JumpNode : public Node {
public:
    JumpNode(long long address) : address_(address) {}

    std::string to_instruction() const override {
        return "JMP " + std::to_string(address_);
    }
private:
    long long address_;
};

class JumpIfPositiveNode : public Node {
public:
    JumpIfPositiveNode(long long address) : address_(address) {}

    std::string to_instruction() const override {
        return "JPOS " + std::to_string(address_);
    }
private:
    long long address_;
};

class JumpIfZeroNode : public Node {
public:
    JumpIfZeroNode(long long address) : address_(address) {}

    std::string to_instruction() const override {
        return "JZERO " + std::to_string(address_);
    }
private:
    long long address_;
};

class CallNode : public Node {
public:
    CallNode(long long address) : address_(address) {}

    std::string to_instruction() const override {
        return "CALL " + std::to_string(address_);
    }
private:
    long long address_;
};

class ReturnNode : public Node {
public:
    std::string to_instruction() const override {
        return "RTRN";
    }
};

class HaltNode : public Node {
public:
    std::string to_instruction() const override {
        return "HALT";
    }
};

} // namespace AsmAST