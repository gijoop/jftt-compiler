#pragma once

#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "optimizer/optimizer.hpp"
#include "asm/asm.hpp"
#include "types.hpp"

namespace Optimizer {

// Applies local optimizations on small windows of instructions
class PeepholePass : public AsmPass {
public:
    std::string name() const override { return "Peephole"; }

    void optimize(std::vector<Asm::Instruction>& code) override {
        if (code.empty()) return;
        
        bool changed = true;
        int iteration = 0;
        const int max_iterations = 100;
        
        while (changed && iteration < max_iterations) {
            changed = false;
            iteration++;
            
            // Mark instructions for deletion
            std::vector<bool> deleted(code.size(), false);

            for (size_t i = 0; i < code.size(); ++i) {
                if (deleted[i]) continue;
                
                auto& curr = code[i];

                // Pattern: SWP x; SWP x -> eliminate both (swap twice = identity)
                if (i + 1 < code.size() && !deleted[i + 1]) {
                    auto& next = code[i + 1];
                    
                    if (curr.op == Asm::Code::SWAP && next.op == Asm::Code::SWAP &&
                        curr.reg == next.reg) {
                        deleted[i] = true;
                        deleted[i + 1] = true;
                        changed = true;
                        continue;
                    }

                    // Pattern: STORE x; LOAD x -> STORE x (value still in RA)
                    if (curr.op == Asm::Code::STORE && next.op == Asm::Code::LOAD &&
                        curr.value == next.value) {
                        deleted[i + 1] = true;
                        changed = true;
                        continue;
                    }

                    // Pattern: JUMP L; LABEL L -> remove JUMP (jumping to next instruction)
                    if (curr.op == Asm::Code::JUMP && next.op == Asm::Code::LABEL &&
                        curr.label && next.label && curr.label->name == next.label->name) {
                        deleted[i] = true;
                        changed = true;
                        continue;
                    }

                    // Pattern: RESET a; ADD a -> RST a (adding 0)
                    if (curr.op == Asm::Code::RESET && next.op == Asm::Code::ADD &&
                        curr.reg == Register::RA && next.reg == Register::RA) {
                        deleted[i + 1] = true;
                        changed = true;
                        continue;
                    }
                }
            }

            // Remove deleted instructions
            size_t deleted_count = std::count(deleted.begin(), deleted.end(), true);
            if (deleted_count > 0) {
                std::vector<Asm::Instruction> new_code;
                new_code.reserve(code.size() - deleted_count);
                for (size_t i = 0; i < code.size(); ++i) {
                    if (!deleted[i]) {
                        new_code.push_back(std::move(code[i]));
                    }
                }
                code = std::move(new_code);
            }
        }
    }
};

// Optimizes chains of jumps (e.g., JUMP L1 where L1: JUMP L2 -> JUMP L2)
class JumpChainPass : public AsmPass {
public:
    std::string name() const override { return "JumpChain"; }

    void optimize(std::vector<Asm::Instruction>& code) override {
        if (code.empty()) return;
        
        // Build label -> position map
        std::unordered_map<std::string, size_t> label_pos;
        for (size_t i = 0; i < code.size(); ++i) {
            if (code[i].op == Asm::Code::LABEL && code[i].label) {
                label_pos[code[i].label->name] = i;
            }
        }

        // Optimize jump chains
        bool changed = true;
        int iterations = 0;
        const int max_iterations = 10;

        while (changed && iterations < max_iterations) {
            changed = false;
            iterations++;

            for (auto& instr : code) {
                if (!instr.label) continue;
                if (instr.op != Asm::Code::JUMP) continue;

                // Find target
                auto it = label_pos.find(instr.label->name);
                if (it == label_pos.end()) continue;

                size_t target_pos = it->second;
                
                // Look for unconditional jump immediately after label
                if (target_pos + 1 < code.size()) {
                    auto& target_next = code[target_pos + 1];
                    if (target_next.op == Asm::Code::JUMP && target_next.label) {
                        // Jump to jump -> skip intermediate
                        if (instr.label->name != target_next.label->name) {
                            instr.label = target_next.label;
                            changed = true;
                        }
                    }
                }
            }
        }
    }
};

// Removes labels that are never referenced
class DeadLabelPass : public AsmPass {
public:
    std::string name() const override { return "DeadLabel"; }

    void optimize(std::vector<Asm::Instruction>& code) override {
        if (code.empty()) return;
        
        // Collect all referenced labels
        std::unordered_set<std::string> referenced;
        for (const auto& instr : code) {
            if (instr.label && instr.op != Asm::Code::LABEL) {
                referenced.insert(instr.label->name);
            }
        }

        // Remove unreferenced labels (but keep important ones like "main")
        code.erase(
            std::remove_if(code.begin(), code.end(), [&](const Asm::Instruction& instr) {
                if (instr.op != Asm::Code::LABEL) return false;
                if (!instr.label) return false;
                // Keep main label and procedure entry labels
                if (instr.label->name == "main") return false;
                // Keep labels that don't start with 'L' (procedure names, etc.)
                if (!instr.label->name.empty() && instr.label->name[0] != 'L' &&
                    instr.label->name.find("_") == std::string::npos) return false;
                return referenced.find(instr.label->name) == referenced.end();
            }),
            code.end()
        );
    }
};


// Combines sequences of instructions into more efficient ones
class InstructionCombinePass : public AsmPass {
public:
    std::string name() const override { return "InstructionCombine"; }

    void optimize(std::vector<Asm::Instruction>& code) override {
        if (code.size() < 4) return;
        
        bool changed = true;
        int iterations = 0;
        const int max_iterations = 10;
        
        while (changed && iterations < max_iterations) {
            changed = false;
            iterations++;
            std::vector<bool> deleted(code.size(), false);

            for (size_t i = 0; i + 3 < code.size(); ++i) {
                if (deleted[i]) continue;

                auto& i0 = code[i];
                auto& i1 = code[i + 1];
                auto& i2 = code[i + 2];
                auto& i3 = code[i + 3];

                // Pattern: RST a; ADD b; STORE x; LOAD x -> RST a; ADD b; STORE x
                // (value is already in RA after the sequence)
                if (i0.op == Asm::Code::RESET && i0.reg == Register::RA &&
                    i1.op == Asm::Code::ADD &&
                    i2.op == Asm::Code::STORE &&
                    i3.op == Asm::Code::LOAD && i2.value == i3.value &&
                    !deleted[i + 1] && !deleted[i + 2] && !deleted[i + 3]) {
                    deleted[i + 3] = true;
                    changed = true;
                }
            }

            // Remove deleted instructions
            size_t deleted_count = std::count(deleted.begin(), deleted.end(), true);
            if (deleted_count > 0) {
                std::vector<Asm::Instruction> new_code;
                new_code.reserve(code.size() - deleted_count);
                for (size_t i = 0; i < code.size(); ++i) {
                    if (!deleted[i]) {
                        new_code.push_back(std::move(code[i]));
                    }
                }
                code = std::move(new_code);
            }
        }
    }
};

// Factory function to create default ASM optimizer
inline AsmOptimizer create_default_asm_optimizer() {
    AsmOptimizer optimizer;
    
    optimizer.add_pass(std::make_unique<PeepholePass>());
    optimizer.add_pass(std::make_unique<JumpChainPass>());
    optimizer.add_pass(std::make_unique<InstructionCombinePass>());
    optimizer.add_pass(std::make_unique<DeadLabelPass>());
    
    // Second iteration for cascading optimizations
    optimizer.add_pass(std::make_unique<PeepholePass>());
    
    return optimizer;
}

} // namespace Optimizer
