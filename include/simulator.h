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
    };

    class Model {
    private:
        int32_t registers_[32];
        size_t pc_;
        const int* user_program_ = nullptr;

        char rs1_, rs2_, op_code_, funct3_, funct7_, rd_;
        int immediate_;

    private:
        int read_op_code_(int cmd);
        int parse_I_type_(int command);
        // I-Type start
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

    public:
        // serves as initialization
        Model(const void* ptr_to_program);

        void execute();
    };
};

int get_bytes(int src, int start, int end);
