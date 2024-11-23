#include "simulator.h"
#include <cassert>
#include <iostream>


int get_bits(int src, int start, int end) {
    return (src & (((1 << (end + 1)) - 1) - ((1 << start) - 1))) >> start;
}

int get_bits(unsigned int src, int start, int end) {
    return (src & (((1 << (end + 1)) - 1) - ((1 << start) - 1))) >> start;
}

int simulator::Model::read_op_code_(int cmd) {
        //                              op_code starts here
        //                              |
        //                              |
        //                              V
        // 00000000 00000000 00000000 00000000
        return cmd & ((1 << 7) - 1);
}

// returns modified instruction
int simulator::Model::parse_I_type_(int cmd) {
    u_immediate_ = get_bits(static_cast<unsigned int>(cmd), 20, 31);
    immediate_   = get_bits(cmd, 20, 31);
    rs1_         = get_bits(cmd, 15, 19);
    funct3_      = get_bits(cmd, 12, 14);
    rd_          = get_bits(cmd, 7 , 11);
    return op_code_ + (funct3_ << 7);
}

int simulator::Model::parse_U_type_(int cmd) {
    u_immediate_ = get_bits(static_cast<unsigned int>(cmd), 12, 31);
    immediate_ = get_bits(cmd, 12, 31);
    rd_        = get_bits(cmd, 7 , 11);
    return op_code_;
}

int simulator::Model::parse_R_type_(int cmd) {
    funct7_ = get_bits(cmd, 25, 31);
    rs2_    = get_bits(cmd, 20, 24);
    rs1_    = get_bits(cmd, 15, 19);
    funct3_ = get_bits(cmd, 12, 14);
    rd_     = get_bits(cmd, 7 , 11);
    return op_code_ + (funct3_ << 7) + (funct7_ << 10);
}

simulator::Model::Model(const char* ptr_to_program): pc_(ptr_to_program) {}

void simulator::Model::execute() {
    int command = *reinterpret_cast<const int*>(pc_);

    op_code_ = read_op_code_(command);

    int modified_op_code = op_code_;
    switch (op_code_) {
        case (simulator::instructions::I_TYPE_INST):
            modified_op_code = parse_I_type_(command);
            break;
        case (simulator::instructions::AUIPC):
        case (simulator::instructions::LUI  ):
            modified_op_code = parse_U_type_(command);
            break;
    }
    switch (modified_op_code) {
        case (simulator::instructions::ADDI): {
            ADDI_();
            break;
        }
        case (simulator::instructions::SLLI): {
            SLLI_();
            break;
        }
        case (simulator::instructions::SLTI): {
            SLTI_();
            break;
        }
        case (simulator::instructions::SLTI_U): {
            SLTI_U_();
            break;
        }
        case (simulator::instructions::XORI): {
            XORI_();
            break;
        }
        case (simulator::instructions::SR_LA_I): {
            SR_LA_I_();
            break;
        }
        case (simulator::instructions::ORI):  {
            ORI_();
            break;
        }
        case (simulator::instructions::ANDI): {
            ANDI_();
            break;
        }
        case (simulator::instructions::AUIPC): {
            AUIPC_();
            break;
        }
        case (simulator::instructions::LUI): {
            LUI_();
            break;
        }
        case (simulator::instructions::ADD): {
            ADD_();
            break;
        }
        case (simulator::instructions::SUB): {
            SUB_();
            break;
        }
        case (simulator::instructions::SLL): {
            SLL_();
            break;
        }
        case (simulator::instructions::SLT_U): {
            SLT_U_();
            break;
        }
        case (simulator::instructions::AND): {
            AND_();
            break;
        }
        case (simulator::instructions::OR): {
            OR_();
            break;
        }
        case (simulator::instructions::XOR): {
            XOR_();
            break;
        }
        case (simulator::instructions::SRL): {
            SRL_();
            break;
        }
        case (simulator::instructions::SRA): {
            SRA_();
            break;
        }
    }

    pc_+=4;
}

void simulator::Model::ADDI_() {
    registers_[rd_] += registers_[rs1_] + immediate_;
    registers_[0] = 0;
}

void simulator::Model::SLLI_() {
    // shift must be located in first 5 bits of immediate
    assert((immediate_ >> 5) == 0);

    registers_[rd_] = registers_[rs1_] << immediate_;

    registers_[0] = 0;
}

void simulator::Model::SLTI_() {
    if (registers_[rs1_] < immediate_)
        registers_[rd_] = 1;
    else
        registers_[rd_] = 0;

    registers_[0] = 0;
}

void simulator::Model::SLTI_U_() {
    if (static_cast<unsigned int>(registers_[rs1_]) < u_immediate_)
        registers_[rd_] = 1;
    else
        registers_[rd_] = 0;

    registers_[0] = 0;
}

void simulator::Model::XORI_() {
    registers_[rd_] = registers_[rs1_] ^ immediate_;
    registers_[0] = 0;
}

void simulator::Model::SR_LA_I_() {
    if (immediate_ & (1 << 10))
        SRAI_();
    else
        SRLI_();
}

void simulator::Model::SRLI_() {
    // immediate_ must look like this 0000000XXXXX
    // so if there is no prefix 0000000, it is illegal instruction
    assert((immediate_ >> 5) == 0);

    registers_[rd_] = registers_[rs1_] >> immediate_;
    registers_[0] = 0;
}

void simulator::Model::SRAI_() {
    // immediate_ must look like this 0100000XXXXX
    // so if there is no prefix 0100000, it is illegal instruction
    assert((immediate_ >> 5) == 0b01000'00);

    int signed_bits = registers_[rs1_] & (1 << 31);

    signed_bits *= get_bits(static_cast<int>((1ULL << 32) - 1), 31 - immediate_, 31);

    registers_[rd_] =  (registers_[rs1_] >> immediate_) | signed_bits;
    registers_[0] = 0;
}

void simulator::Model::ORI_() {
    registers_[rd_] = registers_[rs1_] | immediate_;
    registers_[0] = 0;
}

void simulator::Model::ANDI_() {
    registers_[rd_] = registers_[rs1_] & immediate_;
    registers_[0] = 0;
}

void simulator::Model::LUI_() {
    registers_[rd_] = immediate_ << 12;
    registers_[0] = 0;
}

void simulator::Model::AUIPC_() {
    registers_[rd_] = reinterpret_cast<size_t>(pc_) + (immediate_ << 12);
    registers_[0] = 0;
}

void simulator::Model::ADD_() {
    registers_[rd_] = registers_[rs1_] + registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::SUB_() {
    registers_[rd_] = registers_[rs1_] - registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::SLL_() {
    registers_[rd_] = registers_[rs1_] << get_bits(registers_[rs2_], 0, 4);
    registers_[0] = 0;
}

void simulator::Model::SLT_() {
    registers_[rd_] = registers_[rs1_] < registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::SLT_U_() {
    registers_[rd_] = registers_[rs1_] < registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::AND_() {
    registers_[rd_] = registers_[rs1_] & registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::OR_() {
    registers_[rd_] = registers_[rs1_] | registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::XOR_() {
    registers_[rd_] = registers_[rs1_] ^ registers_[rs2_];
    registers_[0] = 0;
}

void simulator::Model::SRL_() {
    registers_[rd_] = registers_[rs1_] >> get_bits(registers_[rs2_], 0, 4);
    registers_[0] = 0;
}

void simulator::Model::SRA_() {
    int signed_bits = rs1_ & (1 << 31);
    int reg_value = get_bits(registers_[rs2_], 0, 4);;

    signed_bits *= get_bits(static_cast<int>((1LL << 32) - 1), 31 - reg_value, 31);

    registers_[rd_] =  (registers_[rs1_] >> reg_value) | signed_bits;
    registers_[0] = 0;
}

void simulator::Model::get_state(int (&reg)[32], size_t& pc) {
    pc = reinterpret_cast<size_t>(pc_);
    for (int pos = 0; pos < 32; pos++)
        reg[pos] = registers_[pos];
}

void simulator::Model::print_state() {
    std::cout << "-----------Current state of Model-----------\n";
    std::cout << "pc = " << pc_ << "\n";
    for (int i = 0; i < 32; i++)
        std::cout << "x" << i << " = " << registers_[i] << "\n";
    std::cout << "\n";
}
