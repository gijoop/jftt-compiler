#pragma once

#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <algorithm>

#include "optimizer/optimizer.hpp"
#include "tac/tac.hpp"

namespace Optimizer {

// Helper to check if an operand represents a constant value
inline bool is_constant(const Tac::Operand& op) {
    return op.type == Tac::OperandType::CONSTANT;
}

// Helper to get operand key for maps
inline std::string operand_key(const Tac::Operand& op) {
    switch (op.type) {
        case Tac::OperandType::CONSTANT:
            return "const:" + std::to_string(op.value);
        case Tac::OperandType::VARIABLE:
            return "var:" + op.name;
        case Tac::OperandType::TEMP:
            return "tmp:" + std::to_string(op.temp_id);
        case Tac::OperandType::REFERENCE:
            return "ref:" + op.name;
        default:
            return "other:" + op.to_string();
    }
}

inline bool operand_equals(const Tac::Operand& a, const Tac::Operand& b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case Tac::OperandType::CONSTANT:
            return a.value == b.value;
        case Tac::OperandType::VARIABLE:
        case Tac::OperandType::REFERENCE:
        case Tac::OperandType::LABEL:
        case Tac::OperandType::ARRAY:
            return a.name == b.name;
        case Tac::OperandType::TEMP:
            return a.temp_id == b.temp_id;
    }
    return false;
}

// ============================================================================
// Constant Folding Pass
// ============================================================================
// Evaluates operations with constant operands at compile time
class ConstantFoldingPass : public TacPass {
public:
    std::string name() const override { return "ConstantFolding"; }

    void optimize(Tac::Program& program) override {
        for (auto& instr : program.instructions) {
            if (!instr.arg1 || !instr.arg2 || !instr.result) continue;
            if (!is_constant(*instr.arg1) || !is_constant(*instr.arg2)) continue;

            long long left = instr.arg1->value;
            long long right = instr.arg2->value;
            std::optional<long long> result_val;

            switch (instr.op) {
                case Tac::OpCode::ADD:
                    result_val = left + right;
                    break;
                case Tac::OpCode::SUB:
                    result_val = std::max(0LL, left - right);
                    break;
                case Tac::OpCode::MUL:
                    result_val = left * right;
                    break;
                case Tac::OpCode::DIV:
                    if (right != 0) result_val = left / right;
                    else result_val = 0;
                    break;
                case Tac::OpCode::MOD:
                    if (right != 0) result_val = left % right;
                    else result_val = 0;
                    break;
                case Tac::OpCode::EQ:
                    result_val = (left == right) ? 1 : 0;
                    break;
                case Tac::OpCode::NEQ:
                    result_val = (left != right) ? 1 : 0;
                    break;
                case Tac::OpCode::LT:
                    result_val = (left < right) ? 1 : 0;
                    break;
                case Tac::OpCode::GT:
                    result_val = (left > right) ? 1 : 0;
                    break;
                case Tac::OpCode::LTE:
                    result_val = (left <= right) ? 1 : 0;
                    break;
                case Tac::OpCode::GTE:
                    result_val = (left >= right) ? 1 : 0;
                    break;
                default:
                    break;
            }

            if (result_val) {
                // Convert to ASSIGN with constant
                instr.op = Tac::OpCode::ASSIGN;
                instr.arg1 = Tac::Operand::make_const(*result_val);
                instr.arg2 = std::nullopt;
            }
        }
    }
};

// ============================================================================
// Constant Propagation Pass
// ============================================================================
// Propagates known constant values to their uses
class ConstantPropagationPass : public TacPass {
public:
    std::string name() const override { return "ConstantPropagation"; }

    void optimize(Tac::Program& program) override {
        std::unordered_map<std::string, long long> constants;
        std::unordered_set<std::string> labels_seen;

        // First pass: find all labels for control flow analysis
        for (const auto& instr : program.instructions) {
            if (instr.op == Tac::OpCode::LABEL && instr.arg1) {
                labels_seen.insert(instr.arg1->name);
            }
        }

        // Simple constant propagation (invalidates on jumps/labels for safety)
        for (auto& instr : program.instructions) {
            // Invalidate constants at control flow points
            if (instr.op == Tac::OpCode::LABEL || 
                instr.op == Tac::OpCode::JMP ||
                instr.op == Tac::OpCode::JZERO ||
                instr.op == Tac::OpCode::JPOS ||
                instr.op == Tac::OpCode::CALL ||
                instr.op == Tac::OpCode::RETURN ||
                instr.op == Tac::OpCode::FUNC_ENTER ||
                instr.op == Tac::OpCode::FUNC_EXIT) {
                constants.clear();
                continue;
            }

            // Try to propagate constants to operands
            if (instr.arg1 && instr.arg1->type == Tac::OperandType::TEMP) {
                auto key = operand_key(*instr.arg1);
                auto it = constants.find(key);
                if (it != constants.end()) {
                    instr.arg1 = Tac::Operand::make_const(it->second);
                }
            }
            if (instr.arg2 && instr.arg2->type == Tac::OperandType::TEMP) {
                auto key = operand_key(*instr.arg2);
                auto it = constants.find(key);
                if (it != constants.end()) {
                    instr.arg2 = Tac::Operand::make_const(it->second);
                }
            }

            // Track new constant assignments
            if (instr.op == Tac::OpCode::ASSIGN && instr.result && 
                instr.arg1 && is_constant(*instr.arg1)) {
                if (instr.result->type == Tac::OperandType::TEMP) {
                    constants[operand_key(*instr.result)] = instr.arg1->value;
                }
            }
        }
    }
};

// ============================================================================
// Strength Reduction Pass
// ============================================================================
// Replaces expensive operations with cheaper equivalents
class StrengthReductionPass : public TacPass {
public:
    std::string name() const override { return "StrengthReduction"; }

    void optimize(Tac::Program& program) override {
        for (auto& instr : program.instructions) {
            if (!instr.arg1 || !instr.result) continue;

            // Multiplication optimizations
            if (instr.op == Tac::OpCode::MUL && instr.arg2) {
                if (is_constant(*instr.arg2) && is_power_of_2(instr.arg2->value)) {
                    int shift = log2_int(instr.arg2->value);
                    instr.op = Tac::OpCode::SHL_N;
                    instr.arg2 = Tac::Operand::make_const(shift);
                } else if (is_constant(*instr.arg1) && is_power_of_2(instr.arg1->value)) {
                    int shift = log2_int(instr.arg1->value);
                    instr.op = Tac::OpCode::SHL_N;
                    instr.arg1 = instr.arg2;
                    instr.arg2 = Tac::Operand::make_const(shift);
                }
                // Multiply by 0
                else if ((is_constant(*instr.arg1) && instr.arg1->value == 0) ||
                         (is_constant(*instr.arg2) && instr.arg2->value == 0)) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
                // Multiply by 1
                else if (is_constant(*instr.arg2) && instr.arg2->value == 1) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg2 = std::nullopt;
                } else if (is_constant(*instr.arg1) && instr.arg1->value == 1) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = instr.arg2;
                    instr.arg2 = std::nullopt;
                }
            }

            // Division by power of 2 -> shift right
            if (instr.op == Tac::OpCode::DIV && instr.arg2) {
                if (is_constant(*instr.arg2) && is_power_of_2(instr.arg2->value)) {
                    int shift = log2_int(instr.arg2->value);
                    instr.op = Tac::OpCode::SHR_N;
                    instr.arg2 = Tac::Operand::make_const(shift);
                }
                // Division by 1
                else if (is_constant(*instr.arg2) && instr.arg2->value == 1) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg2 = std::nullopt;
                }
                // 0 / x = 0
                else if (is_constant(*instr.arg1) && instr.arg1->value == 0) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
            }

            // Modulo by power of 2 -> optimized modulo
            if (instr.op == Tac::OpCode::MOD && instr.arg2) {
                if (is_constant(*instr.arg2) && is_power_of_2(instr.arg2->value)) {
                    int shift = log2_int(instr.arg2->value);
                    instr.op = Tac::OpCode::MOD_POW2;
                    instr.arg2 = Tac::Operand::make_const(shift);
                }
                // x % 1 = 0
                else if (is_constant(*instr.arg2) && instr.arg2->value == 1) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
                // 0 % x = 0
                else if (is_constant(*instr.arg1) && instr.arg1->value == 0) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
            }

            // Add by small constant -> use INC (cost 1 vs LOAD+ADD which costs ~55)
            if (instr.op == Tac::OpCode::ADD && instr.arg2) {
                if (is_constant(*instr.arg2) && instr.arg2->value == 0) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg2 = std::nullopt;
                } else if (is_constant(*instr.arg1) && instr.arg1->value == 0) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = instr.arg2;
                    instr.arg2 = std::nullopt;
                }
                // Add by 1 -> INC
                else if (is_constant(*instr.arg2) && instr.arg2->value == 1) {
                    instr.op = Tac::OpCode::INC;
                    instr.arg2 = std::nullopt;
                } else if (is_constant(*instr.arg1) && instr.arg1->value == 1) {
                    instr.op = Tac::OpCode::INC;
                    instr.arg1 = instr.arg2;
                    instr.arg2 = std::nullopt;
                }
            }
            
            // Sub by small constant -> use DEC
            if (instr.op == Tac::OpCode::SUB && instr.arg2) {
                if (is_constant(*instr.arg2) && instr.arg2->value == 0) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg2 = std::nullopt;
                }
                // Sub by 1 -> DEC
                else if (is_constant(*instr.arg2) && instr.arg2->value == 1) {
                    instr.op = Tac::OpCode::DEC;
                    instr.arg2 = std::nullopt;
                }
            }
        }
    }

private:
    static bool is_power_of_2(long long n) {
        return n > 0 && (n & (n - 1)) == 0;
    }

    static int log2_int(long long n) {
        int result = 0;
        while (n > 1) {
            n >>= 1;
            result++;
        }
        return result;
    }
};

// ============================================================================
// Algebraic Simplification Pass
// ============================================================================
// Simplifies algebraic identities
class AlgebraicSimplificationPass : public TacPass {
public:
    std::string name() const override { return "AlgebraicSimplification"; }

    void optimize(Tac::Program& program) override {
        for (auto& instr : program.instructions) {
            if (!instr.result) continue;

            // x - x = 0
            if (instr.op == Tac::OpCode::SUB && instr.arg1 && instr.arg2) {
                if (operand_equals(*instr.arg1, *instr.arg2)) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
            }

            // x == x -> 1
            if (instr.op == Tac::OpCode::EQ && instr.arg1 && instr.arg2) {
                if (operand_equals(*instr.arg1, *instr.arg2)) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(1);
                    instr.arg2 = std::nullopt;
                }
            }

            // x != x -> 0
            if (instr.op == Tac::OpCode::NEQ && instr.arg1 && instr.arg2) {
                if (operand_equals(*instr.arg1, *instr.arg2)) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
            }

            // x < x -> 0, x > x -> 0
            if ((instr.op == Tac::OpCode::LT || instr.op == Tac::OpCode::GT) && 
                instr.arg1 && instr.arg2) {
                if (operand_equals(*instr.arg1, *instr.arg2)) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(0);
                    instr.arg2 = std::nullopt;
                }
            }

            // x <= x -> 1, x >= x -> 1
            if ((instr.op == Tac::OpCode::LTE || instr.op == Tac::OpCode::GTE) && 
                instr.arg1 && instr.arg2) {
                if (operand_equals(*instr.arg1, *instr.arg2)) {
                    instr.op = Tac::OpCode::ASSIGN;
                    instr.arg1 = Tac::Operand::make_const(1);
                    instr.arg2 = std::nullopt;
                }
            }
        }
    }
};

// ============================================================================
// Dead Code Elimination Pass
// ============================================================================
// Removes instructions whose results are never used
class DeadCodeEliminationPass : public TacPass {
public:
    std::string name() const override { return "DeadCodeElimination"; }

    void optimize(Tac::Program& program) override {
        bool changed = true;
        while (changed) {
            changed = false;
            
            // Build use set - track all operands that are read/used
            std::unordered_set<std::string> used;
            for (const auto& instr : program.instructions) {
                // arg1 is always a use (if present and is a variable/temp)
                if (instr.arg1 && 
                    (instr.arg1->type == Tac::OperandType::TEMP || 
                     instr.arg1->type == Tac::OperandType::VARIABLE)) {
                    used.insert(operand_key(*instr.arg1));
                }
                // arg2 is always a use (if present and is a variable/temp)
                if (instr.arg2 && 
                    (instr.arg2->type == Tac::OperandType::TEMP || 
                     instr.arg2->type == Tac::OperandType::VARIABLE)) {
                    used.insert(operand_key(*instr.arg2));
                }
                // For ARRAY_ASSIGN, the result field is actually the value being stored (a USE)
                // For JZERO/JPOS, arg1 is the condition value (a USE) - already handled above
                if (instr.op == Tac::OpCode::ARRAY_ASSIGN && instr.result &&
                    (instr.result->type == Tac::OperandType::TEMP ||
                     instr.result->type == Tac::OperandType::VARIABLE)) {
                    used.insert(operand_key(*instr.result));
                }
            }

            // Mark dead code as NOP
            for (auto& instr : program.instructions) {
                // Skip instructions with side effects
                if (has_side_effects(instr.op)) continue;
                
                // Only eliminate dead TEMPs (not variables - they might be needed later)
                if (instr.result && instr.result->type == Tac::OperandType::TEMP) {
                    auto key = operand_key(*instr.result);
                    if (used.find(key) == used.end()) {
                        instr.op = Tac::OpCode::NOP;
                        instr.arg1 = std::nullopt;
                        instr.arg2 = std::nullopt;
                        instr.result = std::nullopt;
                        changed = true;
                    }
                }
            }
        }

        // Remove NOPs
        program.instructions.erase(
            std::remove_if(program.instructions.begin(), program.instructions.end(),
                [](const Tac::Instruction& i) { return i.op == Tac::OpCode::NOP; }),
            program.instructions.end()
        );
    }

private:
    static bool has_side_effects(Tac::OpCode op) {
        switch (op) {
            case Tac::OpCode::WRITE:
            case Tac::OpCode::READ:
            case Tac::OpCode::CALL:
            case Tac::OpCode::RETURN:
            case Tac::OpCode::JMP:
            case Tac::OpCode::JZERO:
            case Tac::OpCode::JPOS:
            case Tac::OpCode::LABEL:
            case Tac::OpCode::HALT:
            case Tac::OpCode::FUNC_ENTER:
            case Tac::OpCode::FUNC_EXIT:
            case Tac::OpCode::PARAM:
            case Tac::OpCode::ARRAY_PARAM:
            case Tac::OpCode::ARRAY_ASSIGN:
                return true;
            default:
                return false;
        }
    }
};

// ============================================================================
// Copy Propagation Pass
// ============================================================================
// Propagates copies: if t1 = t0, replace uses of t1 with t0
class CopyPropagationPass : public TacPass {
public:
    std::string name() const override { return "CopyPropagation"; }

    void optimize(Tac::Program& program) override {
        std::unordered_map<std::string, Tac::Operand> copies;

        for (auto& instr : program.instructions) {
            // Invalidate at control flow points
            if (instr.op == Tac::OpCode::LABEL || 
                instr.op == Tac::OpCode::JMP ||
                instr.op == Tac::OpCode::JZERO ||
                instr.op == Tac::OpCode::JPOS ||
                instr.op == Tac::OpCode::CALL ||
                instr.op == Tac::OpCode::RETURN) {
                copies.clear();
                continue;
            }

            // Propagate copies to arg1
            if (instr.arg1 && instr.arg1->type == Tac::OperandType::TEMP) {
                auto key = operand_key(*instr.arg1);
                auto it = copies.find(key);
                if (it != copies.end()) {
                    instr.arg1 = it->second;
                }
            }

            // Propagate copies to arg2
            if (instr.arg2 && instr.arg2->type == Tac::OperandType::TEMP) {
                auto key = operand_key(*instr.arg2);
                auto it = copies.find(key);
                if (it != copies.end()) {
                    instr.arg2 = it->second;
                }
            }

            // Track simple copies: t1 = ASSIGN t0
            if (instr.op == Tac::OpCode::ASSIGN && 
                instr.result && instr.result->type == Tac::OperandType::TEMP &&
                instr.arg1) {
                copies[operand_key(*instr.result)] = *instr.arg1;
            }

            // Invalidate any copy that is redefined
            if (instr.result) {
                auto key = operand_key(*instr.result);
                // Invalidate all copies that depend on this result
                std::vector<std::string> to_remove;
                for (auto& [k, v] : copies) {
                    if (operand_key(v) == key) {
                        to_remove.push_back(k);
                    }
                }
                for (auto& k : to_remove) {
                    copies.erase(k);
                }
            }
        }
    }
};

// ============================================================================
// Factory function to create default TAC optimizer
// ============================================================================
inline TacOptimizer create_default_tac_optimizer() {
    TacOptimizer optimizer;
    
    // First pass
    optimizer.add_pass(std::make_unique<AlgebraicSimplificationPass>());
    optimizer.add_pass(std::make_unique<ConstantFoldingPass>());
    optimizer.add_pass(std::make_unique<StrengthReductionPass>());
    optimizer.add_pass(std::make_unique<DeadCodeEliminationPass>());
    
    // Second pass for cascading optimizations
    optimizer.add_pass(std::make_unique<ConstantFoldingPass>());
    optimizer.add_pass(std::make_unique<StrengthReductionPass>());
    optimizer.add_pass(std::make_unique<DeadCodeEliminationPass>());
    
    return optimizer;
}

} // namespace Optimizer
