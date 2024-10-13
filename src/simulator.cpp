#include "simulator.h"
#include <cassert>


int get_bytes(int src, int start, int end) {
    return src & (((1 << (end + 1)) - 1) - ((1 << start) - 1));
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
    immediate_  = get_bytes(cmd, 20, 31);
    rs1_        = get_bytes(cmd, 15, 19);
    funct3_     = get_bytes(cmd, 12, 14);
    rd_         = get_bytes(cmd, 7 , 11);
    return op_code_ + (funct3_ << 7);
}

simulator::Model::Model(const void* ptr_to_program) {

    assert((size_t)(ptr_to_program) % 4 == 0);

    user_program_ = static_cast<const int*>(ptr_to_program);
}

void simulator::Model::execute() {
    int command = user_program_[pc_];

    op_code_ = read_op_code_(command);

    int modified_op_code = op_code_;

    switch (op_code_) {
        case (simulator::instructions::I_TYPE_INST): {
            modified_op_code = parse_I_type_(command);
            break;
        }
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

    }

    pc_++;
}

void simulator::Model::ADDI_() {
    registers_[rd_] += registers_[rs1_] + immediate_;
    registers_[0] = 0;
}

void simulator::Model::SLLI_() {
    // shift must be located in first 5 bits of immediate
    assert((immediate_ >> 5) == 0);

    registers_[rd_] = rs1_ << immediate_;

    registers_[0] = 0;
}

void simulator::Model::SLTI_() {
    if ((int) registers_[rs1_] < (int) immediate_)
        registers_[rd_] = 1;
    else
        registers_[rd_] = 0;

    registers_[0] = 0;
}

void simulator::Model::SLTI_U_() {
    if ((unsigned int) registers_[rs1_] < (unsigned int) immediate_)
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

    registers_[rd_] = rs1_ >> immediate_;
    registers_[0] = 0;
}

void simulator::Model::SRAI_() {
    // immediate_ must look like this 0100000XXXXX
    // so if there is no prefix 0100000, it is illegal instruction
    assert((immediate_ >> 5) == 0b01000'00);

    registers_[rd_] = rs1_ >> immediate_;
    registers_[rd_] = get_bytes(31 - immediate_, 31, ((long long int)1 << 32) - 1);
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
