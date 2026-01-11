#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "tac/tac.hpp"
#include "asm/asm.hpp"
#include "symbol_table.hpp"

using namespace Asm;

class AsmGenerator {
    using ASMCode = std::vector<Asm::Instruction>;

    Tac::Program& tac_program;
    ASMCode asm_code;

    void load_to_ra(Tac::Operand op) {
        if (op.type == Tac::OperandType::CONSTANT) {
            asm_code.push_back(make(Code::RESET, Register::RA));
        
            if (op.value == 0) return;

            std::vector<bool> bits;
            bits.reserve(64);
            long long temp = op.value;

            while (temp > 0) {
                bits.push_back(temp % 2 != 0);
                temp /= 2;
            }

            for (int i = bits.size() - 1; i >= 0; --i) {
                if (bits[i]) {
                    asm_code.push_back(make(Code::INC, Register::RA));
                }
                if (i > 0) {
                    asm_code.push_back(make(Code::SHL, Register::RA));
                }
            }
        } else if (op.type == Tac::OperandType::REFERENCE) {
            long long param_addr = SymbolTable::get_address(op);
            asm_code.push_back(Asm::make(Code::LOAD, param_addr));  // ra = address stored in param
            asm_code.push_back(Asm::make(Code::RLOAD, Register::RA)); // ra = value at that address
        } else {
            asm_code.push_back(Asm::make(Code::LOAD, SymbolTable::get_address(op)));
        }
    }

    void store_ra(Tac::Operand op) {
        if (op.type == Tac::OperandType::REFERENCE) {
            asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // Save value to rb
            long long param_addr = SymbolTable::get_address(op);
            asm_code.push_back(Asm::make(Code::LOAD, param_addr));   // ra = target address
            asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // ra = value, rb = address
            asm_code.push_back(Asm::make(Code::RSTORE, Register::RB)); // mem[rb] = ra
        } else {
            asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(op)));
        }
    }

    void copy_to_ra(Register reg) {
        if (reg == Register::RA) return;

        asm_code.push_back(make(Code::RESET, Register::RA));
        asm_code.push_back(make(Code::ADD, reg));
    }

    // Load a constant value into a specific register
    void load_const_to_reg(long long val, Register reg) {
        asm_code.push_back(make(Code::RESET, reg));
        
        if (val == 0) return;

        std::vector<bool> bits;
        bits.reserve(64);
        long long temp = val;

        while (temp > 0) {
            bits.push_back(temp % 2 != 0);
            temp /= 2;
        }

        for (int i = bits.size() - 1; i >= 0; --i) {
            if (bits[i]) {
                asm_code.push_back(make(Code::INC, reg));
            }
            if (i > 0) {
                asm_code.push_back(make(Code::SHL, reg));
            }
        }
    }

    void calc_array_address(Tac::Operand arr_op, Tac::Operand index_op) {
        // Load index into RA
        load_to_ra(index_op);
        asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // ra = index

        if (arr_op.type == Tac::OperandType::REFERENCE) {
            long long param_addr = SymbolTable::get_address(arr_op);
            asm_code.push_back(Asm::make(Code::LOAD, param_addr)); // ra = base address stored in param
        } else if (arr_op.type == Tac::OperandType::ARRAY) {
            long long base_addr = SymbolTable::get_address(arr_op);
            long long start_index = SymbolTable::get_array_start_index(arr_op.name);
            
            // ra = base_addr - start_index (so that base_addr + index - start_index = correct address)
            load_const_to_reg(base_addr - start_index, Register::RA);
        }
        
        asm_code.push_back(Asm::make(Code::ADD, Register::RB)); // ra = base + index - start_index
    }

public:
    AsmGenerator(Tac::Program& p) : tac_program(p) {}

    ASMCode compile() {
        std::unordered_map<std::string, std::shared_ptr<Asm::Label>> labels;

        for(const auto& instr : tac_program.instructions) {
            if (instr.op == Tac::OpCode::LABEL) {
                labels[instr.arg1->name] = std::make_shared<Asm::Label>(instr.arg1->name);
            }
        }

        for(const auto& instr : tac_program.instructions) {
            switch(instr.op) {
                case Tac::OpCode::ADD: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB));
                    load_to_ra(*instr.arg2);
                    // TODO: Optimize for constants and increments
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));  // ra = t1 + t0
                    store_ra(*instr.result); // store to t2
                    break;
                }

                case Tac::OpCode::SUB: {
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB));
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB));  // ra = t1 - t0
                    store_ra(*instr.result); // store to t2
                    break;
                }
                
                case Tac::OpCode::MUL: {
                    auto label_loop = std::make_shared<Asm::Label>("MUL_LOOP_START");
                    auto label_bit_zero = std::make_shared<Asm::Label>("MUL_BIT_ZERO");
                    auto label_end = std::make_shared<Asm::Label>("MUL_LOOP_END");

                    load_to_ra(*instr.arg2);   // ra = right
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = right
                    load_to_ra(*instr.arg1);    // ra = left

                    asm_code.push_back(Asm::make(Code::RESET, Register::RC)); // rc = 0 (acc)

                    asm_code.push_back(Asm::make(Code::LABEL, label_loop));
                    asm_code.push_back(Asm::make(Code::JZERO, label_end));

                    // Youngest ra bit test: rd = ra; ra = ra / 2 * 2; rd = rd - ra
                    asm_code.push_back(Asm::make(Code::RESET, Register::RD));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RD));  // rd = 0
                    asm_code.push_back(Asm::make(Code::ADD, Register::RD));   // rd = ra
                    asm_code.push_back(Asm::make(Code::SHR, Register::RA));
                    asm_code.push_back(Asm::make(Code::SHL, Register::RA));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RD));  // ra = old_val, rd = shifted
                    asm_code.push_back(Asm::make(Code::SUB, Register::RD)); // ra = ra - rd (result: 1 or 0)

                    asm_code.push_back(Asm::make(Code::JZERO, label_bit_zero)); // if rd == 0 skip addition
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));   // rc += rb
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC));

                    asm_code.push_back(Asm::make(Code::LABEL, label_bit_zero));
                    // Prepare for next bit
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RD));
                    asm_code.push_back(Asm::make(Code::SHR, Register::RA)); // ra >>= 1
                    asm_code.push_back(Asm::make(Code::SHL, Register::RB));  // rb <<= 1
                    asm_code.push_back(Asm::make(Code::JUMP, label_loop));

                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RC)); // Wynik do ra

                    store_ra(*instr.result);
                    break;
                }

                case Tac::OpCode::DIV:
                case Tac::OpCode::MOD: {
                    // Generate unique labels
                    auto label_align = std::make_shared<Asm::Label>("DIV_ALIGN");
                    auto label_calc  = std::make_shared<Asm::Label>("DIV_CALC");
                    auto label_check = std::make_shared<Asm::Label>("DIV_CHECK");
                    auto label_end   = std::make_shared<Asm::Label>("DIV_END");

                    // Load Divisor (arg2) -> rb
                    load_to_ra(*instr.arg2); 
                    // CHECK ZERO BEFORE SWAP (while value is still in RA)
                    asm_code.push_back(Asm::make(Code::JZERO, label_end)); 
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = Divisor

                    // Load Dividend (arg1) -> RC
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC)); // RC = Dividend (Remainder)

                    // Init Quotient (RD) = 0
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA)); // ra = 0
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RD));  // RD = 0, RA = OldRD

                    // Init Mask (RE) = 1
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA)); // ra = 0
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));   // ra = 1
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RE));  // RE = 1, RA = OldRE

                    // Alignment Phase (Shift Divisor Left)
                    asm_code.push_back(Asm::make(Code::LABEL, label_align));

                    // Check: Is Divisor (RB) > Remainder (RC)?
                    // Calc: RA = rb - RC
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));
                    asm_code.push_back(Asm::make(Code::SUB, Register::RC));
                    // If rb - RC > 0, rb is too big, so we are done aligning.
                    asm_code.push_back(Asm::make(Code::JPOS, label_calc));

                    // Shift Divisor (RB) Left
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));
                    asm_code.push_back(Asm::make(Code::SHL, Register::RA));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB));

                    // Shift Mask (RE) Left
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RE));
                    asm_code.push_back(Asm::make(Code::SHL, Register::RA));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RE));

                    asm_code.push_back(Asm::make(Code::JUMP, label_align));

                    // Calculation (Subtract and Shift Right)
                    asm_code.push_back(Asm::make(Code::LABEL, label_calc));

                    // Loop Condition: If Mask (RE) is 0, we are finished.
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RE));
                    asm_code.push_back(Asm::make(Code::JZERO, label_end));

                    // Check: Does Divisor rb fit in Remainder RC?
                    // We want to know if RC >= rb.
                    // Test: RA = rb - RC.
                    // If RA > 0 (Pos), then rb > RC (Doesn't fit).
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));
                    asm_code.push_back(Asm::make(Code::SUB, Register::RC));
                    asm_code.push_back(Asm::make(Code::JPOS, label_check));

                    // IT FITS:
                    // Remainder = Remainder - Divisor
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RC));
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC));

                    // Quotient = Quotient + Mask
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RD));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RE));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RD));

                    asm_code.push_back(Asm::make(Code::LABEL, label_check));

                    // Shift Divisor (RB) Right
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));
                    asm_code.push_back(Asm::make(Code::SHR, Register::RA));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB));

                    // Shift Mask (RE) Right
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RE));
                    asm_code.push_back(Asm::make(Code::SHR, Register::RA));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RE));

                    asm_code.push_back(Asm::make(Code::JUMP, label_calc));

                    // Finalize
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    if (instr.op == Tac::OpCode::DIV) {
                        asm_code.push_back(Asm::make(Code::ADD, Register::RD)); // Result = Quotient
                    } else {
                        asm_code.push_back(Asm::make(Code::ADD, Register::RC)); // Result = Remainder
                    }
                    
                    store_ra(*instr.result);
                    break;
                }

                case Tac::OpCode::ASSIGN: {
                    load_to_ra(*instr.arg1);

                    store_ra(*instr.result);
                    break;
                }

                case Tac::OpCode::WRITE: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::WRITE));
                    break;
                }

                case Tac::OpCode::READ: {
                    asm_code.push_back(Asm::make(Code::READ));
                    store_ra(*instr.result);
                    break;
                }

                case Tac::OpCode::LABEL: {
                    auto lbl = labels[instr.arg1->name];
                    asm_code.push_back(Asm::make(Code::LABEL, lbl));
                    break;
                }

                case Tac::OpCode::JMP: {
                    auto lbl = labels[instr.arg1->name];
                    asm_code.push_back(Asm::make(Code::JUMP, lbl));
                    break;
                }

                case Tac::OpCode::JZERO: {
                    auto lbl = labels[instr.result->name];
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::JZERO, lbl));
                    break;
                }

                case Tac::OpCode::JPOS: {
                    auto lbl = labels[instr.result->name];
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::JPOS, lbl));
                    break;
                }

                case Tac::OpCode::CALL: {
                    auto lbl = labels[instr.arg1->name];
                    asm_code.push_back(Asm::make(Code::CALL, lbl));
                    break;
                }

                case Tac::OpCode::RETURN: {
                    asm_code.push_back(Asm::make(Code::RETURN));
                    break;
                }

                case Tac::OpCode::FUNC_ENTER: {
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.arg1)));
                    break;
                }

                case Tac::OpCode::FUNC_EXIT: {
                    asm_code.push_back(Asm::make(Code::LOAD, SymbolTable::get_address(*instr.arg1)));
                    break;
                }

                case Tac::OpCode::PARAM: {
                    if (instr.arg1->type == Tac::OperandType::REFERENCE) {
                        long long param_addr = SymbolTable::get_address(*instr.arg1);
                        asm_code.push_back(Asm::make(Code::LOAD, param_addr));  // ra = address stored in the reference
                    } else {
                        long long actual_param_addr = SymbolTable::get_address(*instr.arg1);
                        load_to_ra(Tac::Operand::make_const(actual_param_addr));
                    }
                    
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.arg2)));
                    break;
                }

                case Tac::OpCode::ARRAY_LOAD: {
                    calc_array_address(*instr.arg1, *instr.arg2);
                    // ra = address of arr[index]
                    asm_code.push_back(Asm::make(Code::RLOAD, Register::RA));
                    store_ra(*instr.result);
                    break;
                }

                case Tac::OpCode::ARRAY_ASSIGN: {
                    load_to_ra(*instr.result);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC)); // RC = value to store
                    
                    calc_array_address(*instr.arg1, *instr.arg2);
                    // ra = address of arr[index]
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = address
                    
                    // Store value
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RC)); // ra = value
                    asm_code.push_back(Asm::make(Code::RSTORE, Register::RB)); // mem[RB] = RA
                    break;
                }

                case Tac::OpCode::ARRAY_PARAM: {
                    if (instr.arg1->type == Tac::OperandType::REFERENCE) {
                        // forward the reference
                        long long param_addr = SymbolTable::get_address(*instr.arg1);
                        asm_code.push_back(Asm::make(Code::LOAD, param_addr));
                    } else if (instr.arg1->type == Tac::OperandType::ARRAY) {
                        // compute base address adjusted for start_index
                        long long base_addr = SymbolTable::get_address(*instr.arg1);
                        long long start_index = SymbolTable::get_array_start_index(instr.arg1->name);
                        // pass (base_addr - start_index) so that procedure can use base + index directly
                        load_const_to_reg(base_addr - start_index, Register::RA);
                    }
                    
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.arg2)));
                    break;
                }

                case Tac::OpCode::HALT: {
                    asm_code.push_back(Asm::make(Code::HALT));
                    break;
                }

                case Tac::OpCode::EQ: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = arg1
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC)); // rc = arg2
                    copy_to_ra(Register::RC); // ra = arg2
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB)); // ra = arg2 - arg1
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // ra = arg1, rb = arg2 - arg1
                    asm_code.push_back(Asm::make(Code::SUB, Register::RC)); // ra = arg1 - arg2
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB)); // ra = (arg2 - arg1) + (arg1 - arg2)

                    auto label_set_true = std::make_shared<Asm::Label>("EQ_SET_TRUE");
                    auto label_end = std::make_shared<Asm::Label>("EQ_END");
                    asm_code.push_back(Asm::make(Code::JZERO, label_set_true));
                    // False case
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // True case
                    asm_code.push_back(Asm::make(Code::LABEL, label_set_true));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));
                    // End
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
                    break;
                }
                case Tac::OpCode::NEQ: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = arg1
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC)); // rc = arg2
                    copy_to_ra(Register::RC); // ra = arg2
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB)); // ra = arg2 - arg1
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // ra = arg1, rb = arg2 - arg1
                    asm_code.push_back(Asm::make(Code::SUB, Register::RC)); // ra = arg1 - arg2
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB)); // ra = (arg2 - arg1) + (arg1 - arg2)

                    auto label_set_true = std::make_shared<Asm::Label>("NEQ_SET_TRUE");
                    auto label_end = std::make_shared<Asm::Label>("NEQ_END");
                    asm_code.push_back(Asm::make(Code::JPOS, label_set_true));
                    // False case
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // True case
                    asm_code.push_back(Asm::make(Code::LABEL, label_set_true));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));
                    // End
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    store_ra(*instr.result);
                    break;
                }
                case Tac::OpCode::LT: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = arg1
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB)); // ra = arg1 - arg2

                    auto label_set_true = std::make_shared<Asm::Label>("LT_SET_TRUE");
                    auto label_end = std::make_shared<Asm::Label>("LT_END");
                    asm_code.push_back(Asm::make(Code::JPOS, label_set_true));
                    // False case
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // True case
                    asm_code.push_back(Asm::make(Code::LABEL, label_set_true));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));
                    // End
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    store_ra(*instr.result);
                    break;
                }
                case Tac::OpCode::GT: {
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = arg2
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB)); // ra = arg2 - arg1

                    auto label_set_true = std::make_shared<Asm::Label>("GT_SET_TRUE");
                    auto label_end = std::make_shared<Asm::Label>("GT_END");
                    asm_code.push_back(Asm::make(Code::JPOS, label_set_true));
                    // False case
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // True case
                    asm_code.push_back(Asm::make(Code::LABEL, label_set_true));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));
                    // End
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    store_ra(*instr.result);
                    break;
                }
                case Tac::OpCode::LTE: {
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = arg2
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB)); // ra = arg2 - arg1

                    auto label_set_false = std::make_shared<Asm::Label>("LTE_SET_FALSE");
                    auto label_end = std::make_shared<Asm::Label>("LTE_END");
                    asm_code.push_back(Asm::make(Code::JPOS, label_set_false));
                    // True case
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // False case
                    asm_code.push_back(Asm::make(Code::LABEL, label_set_false));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    // End
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    store_ra(*instr.result);
                    break;
                }
                case Tac::OpCode::GTE: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = arg1
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB)); // ra = arg1 - arg2

                    auto label_set_false = std::make_shared<Asm::Label>("GTE_SET_FALSE");
                    auto label_end = std::make_shared<Asm::Label>("GTE_END");
                    asm_code.push_back(Asm::make(Code::JPOS, label_set_false));
                    // True case
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // False case
                    asm_code.push_back(Asm::make(Code::LABEL, label_set_false));
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::JUMP, label_end));
                    // End
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    store_ra(*instr.result);
                    break;
                }

                default:
                    std::cerr << "Warning: Unsupported TAC operation in compilation to ASM." << std::endl;
                    break;
            }
        }

        // Assign addresses to labels
        long long address_counter = 0;
        for (auto& instr : asm_code) {
            if (instr.op == Code::LABEL) {
                instr.label->address = address_counter;
            } else {
                address_counter++;
            }
        }
        
        return std::move(asm_code);
    }
};