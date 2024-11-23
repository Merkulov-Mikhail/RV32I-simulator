#include <cstdint>
#include <cstddef>

namespace simulator {

    namespace instructions {
        const int I_TYPE_INST = 0b0010011;
    //-------------------------------------
    //          I_TYPE Start
    //-------------------------------------
        const int ADDI    = 0b000'0010011;
        const int SLLI    = 0b001'0010011;
        const int SLTI    = 0b010'0010011;
        const int SLTI_U  = 0b011'0010011;
        const int XORI    = 0b100'0010011;
        const int SR_LA_I = 0b101'0010011;
        const int ORI     = 0b110'0010011;
        const int ANDI    = 0b111'0010011;
    //-------------------------------------
    //          I_TYPE End
    //-------------------------------------
        const int LUI     = 0b0110111;
        const int AUIPC   = 0b0010111;


        const int R_TYPE_INST = 0b0110011;
    //-------------------------------------
    //          R_TYPE Start
    //-------------------------------------
        const int ADD   = 0b0000000'000'0110011;
        const int SLT   = 0b0000000'010'0110011;
        const int SLT_U = 0b0000000'011'0110011;
        const int AND   = 0b0000000'111'0110011; // not implemented
        const int OR    = 0b0000000'110'0110011; // not implemented
        const int XOR   = 0b0000000'100'0110011; // not implemented
        const int SLL   = 0b0000000'001'0110011;
        const int SRL   = 0b0000000'101'0110011; // not implemented
        const int SUB   = 0b0100000'000'0110011;
        const int SRA   = 0b0100000'101'0110011; // not implemented
    };

    class Model {
    private:
        int32_t registers_[32]{};
        const char* pc_ = nullptr;

        char rs1_, rs2_, op_code_, funct3_, funct7_, rd_;
        int immediate_;
        unsigned int u_immediate_;

    private:
        int read_op_code_(int cmd);
        int parse_I_type_(int command);
        int parse_U_type_(int command);
        int parse_R_type_(int command);
        // I-Type Start
        void ADDI_   ();
        void SLLI_   ();
        void SLTI_   ();
        void SLTI_U_ ();
        void XORI_   ();
        void SR_LA_I_();
        void SRLI_   ();
        void SRAI_   ();
        void ORI_    ();
        void ANDI_   ();
        // I-Type End

        void LUI_    ();
        void AUIPC_  ();

        // R-Type Start
        void ADD_    ();
        void SLT_    ();
        void SLT_U_  ();
        void AND_    ();
        void OR_     ();
        void XOR_    ();
        void SLL_    ();
        void SRL_    ();
        void SUB_    ();
        void SRA_    ();
        // R-Type End

    public:
        // serves as initialization
        Model(const char* ptr_to_program);

        void execute();

        void get_state(int (&reg)[32], size_t& pc);
        void print_state();
    };
};

int get_bits(int src, int start, int end);
