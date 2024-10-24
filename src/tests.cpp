#include <iostream>
#include "simulator.h"
#include <cassert>

void pass(const char* str) {
    std::cout << str << "\t\033[32mpassed\033[0m\n";
}

void test_ADDI() {
    int registers[32];
    size_t pc;
    int* cmd = (int*) calloc(sizeof(int), 2);
    cmd[0] = 0b000000000011'00000'000'00111'0010011; // x7 += 3
    cmd[1] = 0b000000000111'00000'000'00111'0010011; // x7 += 7

    simulator::Model m {cmd};
    m.execute();
    m.get_state(registers, pc);
    assert(registers[7] == 3 && pc == 1);
    m.execute();
    m.get_state(registers, pc);
    assert(registers[7] == 10 && pc == 2);
    for (int i = 0; i < 7; i++)
        assert(registers[i] == 0);
    for (int i = 8; i < 32; i++)
        assert(registers[i] == 0);
    pass("ADDI");
}

void test_SLLI() {
    int registers[32];
    size_t pc;
    int cmd[2] = {0b000000000011'00000'000'01111'0010011, // x15 = 3
                  0b000000000111'01111'001'00111'0010011}; // x7 = x15 << 7 = 384

    simulator::Model m {cmd};
    m.execute();
    m.get_state(registers, pc);
    assert(registers[15] == 3 && pc == 1);
    m.execute();
    m.get_state(registers, pc);
    assert(registers[7] == 384 && pc == 2);
    pass("SLLI");
}

void test_SLTI() {
    int registers[32];
    size_t pc;

    int cmd[4] = {0b000011111111'00000'000'00101'0010011, // x5 = 255
                  0b000011111110'00101'010'00011'0010011, // x3 = x5 < 254 = 0
                  0b000100000000'00101'010'00100'0010011, // x4 = x5 < 256 = 1
             (int)0b100000000000'00101'010'00110'0010011};// x6 = x5 < -2048 = 0

    simulator::Model m {cmd};
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.get_state(registers, pc);
    assert(registers[3] == 0 && registers[5] == 255 && registers[4] == 1  && registers[6] == 0);
    pass("SLTI");
}

void test_SLTI_U() {
    int registers[32];
    size_t pc;

    int cmd[9] = {0b000011111111'00000'000'00101'0010011, // x5 = 255
                  0b000011111110'00101'011'00011'0010011, // x3 = x5 < 254 = 0
                  0b000100000000'00101'011'00100'0010011, // x4 = x5 < 256 = 1
             (int)0b100000000000'00101'011'00110'0010011, // x6 = x5 < 4096 = 1
             (int)0b010000000000'00000'000'00101'0010011, // x5 += 1024
             (int)0b010000000000'00000'000'00101'0010011, // x5 += 1024
             (int)0b010000000000'00000'000'00101'0010011, // x5 += 1024
             (int)0b010000000000'00000'000'00101'0010011, // x5 += 1024 = 255 + 4096
             (int)0b100000000000'00101'011'00111'0010011};// x7 = x5 < 4096 = 0

    simulator::Model m {cmd};
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.execute();
    m.get_state(registers, pc);
    assert(registers[3] == 0 && registers[5] == 255 + 4096 && registers[4] == 1  && registers[6] == 1 && registers[7] == 0);
    pass("SLTI_U");
}

void test_XORI_() {
    int registers[32];
    size_t pc;

    int cmd[3] = {0b000011111111'00000'000'00101'0010011, // x5 = 255
                  0b000000000011'00101'100'00100'0010011, // x4 = x5 ^ 3 = 252
                  0b000000000011'00100'100'00100'0010011};// x4 = x4 ^ 3 = 255

    simulator::Model m {cmd};
    m.execute();
    m.execute();
    m.get_state(registers, pc);
    assert(registers[4] == 252 && registers[5] == 255);
    m.execute();
    m.get_state(registers, pc);
    assert(registers[4] == 255 && registers[5] == 255);
    pass("XORI");
}

void test_SRLI() {
    int registers[32];
    size_t pc;

    int cmd[3] = {(int)0b000011111111'00000'000'00101'0010011, // x5 = 255
                       0b000000000011'00101'101'00101'0010011, // x5 = x5 >> 3 = 31
                       0b000000000100'00101'101'00110'0010011};// x6 = x5 >> 4 = 1

    simulator::Model m {cmd};
    m.execute();
    m.execute();
    m.get_state(registers, pc);
    assert(registers[5] == 31 && pc == 2);
    m.execute();
    m.get_state(registers, pc);
    assert(registers[6] == 1 && pc == 3 && registers[5] == 31);
    pass("SRLI");
}

void test_SRAI() {
    int registers[32];
    size_t pc;

    int cmd[3] = {(int)0b000011111111'00000'000'00101'0010011, // x5 = 255
                       0b000000000011'00101'101'00101'0010011, // x5 = x5 >> 3 = 31
                       0b000000000100'00101'101'00110'0010011};// x6 = x5 >> 4 = 1

    simulator::Model m {cmd};

    m.execute();
    m.execute();
    m.get_state(registers, pc);
    assert(registers[5] == 31 && pc == 2);
    m.execute();
    m.get_state(registers, pc);
    assert(registers[6] == 1 && pc == 3 && registers[5] == 31);
    pass("SRAI");
}

int main() {
    test_ADDI();
    test_SLLI();
    test_SLTI();
    test_SLTI_U();
    test_XORI_();
    test_SRLI();
    test_SRAI();
}
