#pragma once

#include <vector>
#include <memory>
#include "tac/tac.hpp"
#include "asm/asm.hpp"

namespace Optimizer {

// Base class for TAC optimizations
class TacPass {
public:
    virtual ~TacPass() = default;
    virtual std::string name() const = 0;
    virtual void optimize(Tac::Program& program) = 0;
};

// Base class for ASM optimizations
class AsmPass {
public:
    virtual ~AsmPass() = default;
    virtual std::string name() const = 0;
    virtual void optimize(std::vector<Asm::Instruction>& code) = 0;
};

// TAC Optimizer - runs a pipeline of TAC optimization passes
class TacOptimizer {
public:
    void add_pass(std::unique_ptr<TacPass> pass) {
        passes_.push_back(std::move(pass));
    }

    void optimize(Tac::Program& program) {
        for (auto& pass : passes_) {
            pass->optimize(program);
        }
    }

private:
    std::vector<std::unique_ptr<TacPass>> passes_;
};

// ASM Optimizer - runs a pipeline of ASM optimization passes
class AsmOptimizer {
public:
    void add_pass(std::unique_ptr<AsmPass> pass) {
        passes_.push_back(std::move(pass));
    }

    void optimize(std::vector<Asm::Instruction>& code) {
        for (auto& pass : passes_) {
            pass->optimize(code);
        }
    }

private:
    std::vector<std::unique_ptr<AsmPass>> passes_;
};

} // namespace Optimizer
