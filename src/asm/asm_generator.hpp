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
        } else {
            asm_code.push_back(make(Code::LOAD, SymbolTable::get_address(op)));
        }
    }

    void copy_to_ra(Register reg) {
        if (reg == Register::RA) return;

        asm_code.push_back(make(Code::RESET, Register::RA));
        asm_code.push_back(make(Code::ADD, reg));
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

        // 2. Generowanie kodu
        for(const auto& instr : tac_program.instructions) {
            switch(instr.op) {
                case Tac::OpCode::ADD: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB));
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));  // RA = t1 + t0
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result))); // zapisz do t2
                    break;
                }

                case Tac::OpCode::SUB: {
                    load_to_ra(*instr.arg2);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB));
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB));  // RA = t1 - t0
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result))); // zapisz do t2
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

                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
                    break;
                }

                case Tac::OpCode::DIV:
                case Tac::OpCode::MOD: {
                    // Generate unique labels
                    auto label_align = std::make_shared<Asm::Label>("DIV_ALIGN");
                    auto label_calc  = std::make_shared<Asm::Label>("DIV_CALC");
                    auto label_check = std::make_shared<Asm::Label>("DIV_CHECK");
                    auto label_end   = std::make_shared<Asm::Label>("DIV_END");

                    // ---------------------------------------------------------
                    // 1. SETUP & LOADING (Order is critical!)
                    // ---------------------------------------------------------

                    // A. Load Divisor (arg2) -> RB
                    load_to_ra(*instr.arg2); 
                    // CHECK ZERO BEFORE SWAP (while value is still in RA)
                    asm_code.push_back(Asm::make(Code::JZERO, label_end)); 
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // RB = Divisor

                    // B. Load Dividend (arg1) -> RC
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC)); // RC = Dividend (Remainder)

                    // C. Init Quotient (RD) = 0
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA)); // RA = 0
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RD));  // RD = 0, RA = OldRD

                    // D. Init Mask (RE) = 1
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA)); // RA = 0
                    asm_code.push_back(Asm::make(Code::INC, Register::RA));   // RA = 1
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RE));  // RE = 1, RA = OldRE

                    // ---------------------------------------------------------
                    // 2. ALIGNMENT PHASE (Shift Divisor Left)
                    // ---------------------------------------------------------
                    asm_code.push_back(Asm::make(Code::LABEL, label_align));

                    // Check: Is Divisor (RB) > Remainder (RC)?
                    // Calc: RA = RB - RC
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));
                    asm_code.push_back(Asm::make(Code::SUB, Register::RC));
                    // If RB - RC > 0, RB is too big, so we are done aligning.
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

                    // ---------------------------------------------------------
                    // 3. CALCULATION PHASE (Subtract and Shift Right)
                    // ---------------------------------------------------------
                    asm_code.push_back(Asm::make(Code::LABEL, label_calc));

                    // Loop Condition: If Mask (RE) is 0, we are finished.
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RE));
                    asm_code.push_back(Asm::make(Code::JZERO, label_end));

                    // Check: Does Divisor (RB) fit in Remainder (RC)?
                    // We want to know if RC >= RB.
                    // Test: RA = RB - RC.
                    // If RA > 0 (Pos), then RB > RC (Doesn't fit).
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RB));
                    asm_code.push_back(Asm::make(Code::SUB, Register::RC));
                    asm_code.push_back(Asm::make(Code::JPOS, label_check));

                    // IT FITS:
                    // 1. Remainder = Remainder - Divisor
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    asm_code.push_back(Asm::make(Code::ADD, Register::RC));
                    asm_code.push_back(Asm::make(Code::SUB, Register::RB));
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC));

                    // 2. Quotient = Quotient + Mask
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

                    // ---------------------------------------------------------
                    // 4. FINALIZE (Store Result)
                    // ---------------------------------------------------------
                    asm_code.push_back(Asm::make(Code::LABEL, label_end));
                    
                    asm_code.push_back(Asm::make(Code::RESET, Register::RA));
                    if (instr.op == Tac::OpCode::DIV) {
                        asm_code.push_back(Asm::make(Code::ADD, Register::RD)); // Result = Quotient
                    } else {
                        asm_code.push_back(Asm::make(Code::ADD, Register::RC)); // Result = Remainder
                    }
                    
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
                    break;
                }

                case Tac::OpCode::ASSIGN: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
                    break;
                }

                case Tac::OpCode::WRITE: {
                    load_to_ra(*instr.arg1);
                    asm_code.push_back(Asm::make(Code::WRITE));
                    break;
                }

                case Tac::OpCode::READ: {
                    asm_code.push_back(Asm::make(Code::READ));
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
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
                    load_to_ra(*instr.arg2); // number of arguments
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RB)); // rb = count
                    load_to_ra(*instr.result); // first reference
                    asm_code.push_back(Asm::make(Code::SWAP, Register::RC)); // rc = first_ref
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
                    load_to_ra({Tac::Operand::make_const(SymbolTable::get_address(*instr.arg1))});
                    asm_code.push_back(Asm::make(Code::LOAD, SymbolTable::get_address(*instr.arg2)));
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
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
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
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
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
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
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
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
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
                    asm_code.push_back(Asm::make(Code::STORE, SymbolTable::get_address(*instr.result)));
                    break;
                }

                default:
                    std::cerr << "Warning: Unsupported TAC operation in compilation to ASM." << std::endl;
                    break;
            }
        }

        // 3. Nadpisanie adresów labeli
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