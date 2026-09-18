#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <iostream>
#include <cstdint>

enum Type {
    I,
    R,
    None
};

enum OperandType {
    imm12,
    imm16,
    reg,
    none
};

struct Instruction {
    std::string description;
    Type type;
    std::vector<OperandType> operands;
    uint8_t bitrepr;

    Instruction(std::string description, Type type, std::vector<OperandType> operands, uint8_t bitrepr) : description(description), type(type), operands(operands), bitrepr(bitrepr) {}
};

std::unordered_map<std::string_view, Instruction> instructionList = {
    {"nop",   Instruction("no operation",                                                          None, {},                    0)},
    {"add",   Instruction("r1 = r2 + r3",                                                          R,    {reg, reg, reg},       1)},
    {"sub",   Instruction("r1 = r2 - r3",                                                          R,    {reg, reg, reg},       2)},
    {"mul",   Instruction("r1 = r2 * r3",                                                          R,    {reg, reg, reg},       3)},
    {"div",   Instruction("r1 = r2 / r3, div by zero = exception int, ignored if ints disabled",   R,    {reg, reg, reg},       4)},
    {"inc",   Instruction("r1++",                                                                  R,    {reg},                 5)},
    {"dec",   Instruction("r1--",                                                                  R,    {reg},                 6)},
    {"not",   Instruction("r1 = ~r1",                                                              R,    {reg},                 7)},
    {"or",    Instruction("r1 = r2 | r3",                                                          R,    {reg, reg, reg},       8)},
    {"and",   Instruction("r1 = r2 & r3",                                                          R,    {reg, reg, reg},       9)},
    {"xor",   Instruction("r1 = r2 ^ r3",                                                          R,    {reg, reg, reg},      10)},
    {"lsh",   Instruction("r1 = r2 << r3, bitshift, logical shift",                                R,    {reg, reg, reg},      11)},
    {"rsh",   Instruction("r1 = r2 >> r3, bitshift, logical shift",                                R,    {reg, reg, reg},      12)},
    {"mov",   Instruction("r1 = r2",                                                               R,    {reg, reg},           13)},
    {"ldhi",  Instruction("r1[top 16] = imm16",                                                    I,    {reg, none, imm16},   14)},
    {"ldli",  Instruction("r1[bottom 16] = imm16",                                                 I,    {reg, none, imm16},   15)}, 
    {"push",  Instruction("stack[ptr++] = r1",                                                     R,    {reg},                16)},
    {"pop",   Instruction("r1 = stack[--ptr]",                                               R,    {reg},                17)},
    {"load",  Instruction("r1 = mem[r2] unsigned, r3 byte count, max 4",                           R,    {reg, reg, reg},      18)},
    {"sload", Instruction("r1 = mem[r2] signed, r3 byte count, max 4, auto sign-extends",          R,    {reg, reg, reg},      19)},
    {"store", Instruction("mem[r1] = r2 unsigned / just copies bit pattern, r3 byte count, max 4", R,    {reg, reg, reg},      20)},
    {"out",   Instruction("output[r1] = r2",                                                       R,    {reg, reg},           21)},
    {"in",    Instruction("r1 = input[r2]",                                                        R,    {reg, reg},           22)},
    {"call",  Instruction("push return address, pc = label",                                       I,    {none, none, imm16},  23)},
    {"ret",   Instruction("pc = pop addr",                                                         None, {},                   24)},
    {"iret",  Instruction("pop all registers sequentially, including PC",                          None, {},                   25)},
    {"wfi",   Instruction("wait until interrupt",                                                  None, {},                   26)},
    {"cmp",   Instruction("compare r1 r2, sets appropriate flags (Z/C/O/S)",                       R,    {reg, reg},           27)},
    {"jmp",   Instruction("jump to label unconditionally",                                         I,    {none, none, imm16},  28)},
    {"jz",    Instruction("jump if zero",                                                          I,    {none, none, imm16},  29)},
    {"jnz",   Instruction("jump if not zero",                                                      I,    {none, none, imm16},  30)},
    {"jlu",   Instruction("jump less unsigned",                                                    I,    {none, none, imm16},  31)},
    {"jgu",   Instruction("jump greater unsigned",                                                 I,    {none, none, imm16},  32)},
    {"jls",   Instruction("jump less signed",                                                      I,    {none, none, imm16},  33)},
    {"jgs",   Instruction("jump greater signed",                                                   I,    {none, none, imm16},  34)},
    {"ei",    Instruction("enable all interrupts",                                                 None, {},                   35)},
    {"di",    Instruction("disable all interrupts",                                                None, {},                   36)},
    {"esi",   Instruction("enable specific interrupt id r1",                                       R,    {reg},                37)},
    {"dsi",   Instruction("disable specific interrupt id r1",                                      R,    {reg},                38)},
    {"fi",    Instruction("force interrupt id r1, ignored if disabled",                            R,    {reg},                39)},
    {"jmpr",  Instruction("jump to r1 value unconditionally",                                      R,    {reg},                40)},
    {"jzr",   Instruction("jump to r1 val if zero",                                                R,    {reg},                41)},
    {"jnzr",  Instruction("jump to r1 val if not zero",                                            R,    {reg},                42)},
    {"jlur",  Instruction("jump to r1 val less unsigned",                                          R,    {reg},                43)},
    {"jgur",  Instruction("jump to r1 val greater unsigned",                                       R,    {reg},                44)},
    {"jlsr",  Instruction("jump to r1 val less signed",                                            R,    {reg},                45)},
    {"jgsr",  Instruction("jump to r1 val greater signed",                                         R,    {reg},                46)},
    {"callr", Instruction("push return address, pc = r1",                                          R,    {reg},                47)},
    {"se",    Instruction("sign extend r1 = r2, r3 source bit count",                              R,    {reg, reg, reg},      48)},
    {"hold",  Instruction("temp-push (not stack) r1, restore after imm16 instructions",            I,    {reg, none, imm16},   49)},
    {"bitct", Instruction("r1 = r2 '1' bit count",                                                 R,    {reg, reg},           50)},
    {"swap",  Instruction("r1 = r2, r2 = r1, uses internal temp storage",                          R,    {reg, reg},           51)},
};

int main() {
    std::cout << "Enter an opcode to see its information.\n'list': list opcodes\n'info': show architecture info\n'quit': exit program\n";
    
    while (1) {
        std::string written = "";
        std::cout << "> ";
        std::cin >> written;
        if (written == "quit") {
            return 0;
        } else if (written == "list") {
            for (auto& elem : instructionList) {
                std::cout << " - " << elem.first << '\n';
            }
        } else if (written == "info") {
            std::cout << "  Architecture:      32-bit\n";
            std::cout << "  Version:           1.0\n";
            std::cout << "  Register count:    16 (r0-r15)\n";
            std::cout << "  Flags:             Z C O S\n";
            std::cout << "  Instruction width: 4 bytes\n";
            std::cout << "  Addressing width:  4 bytes\n";
            std::cout << "  Immediate sizes:   12-bit, 16-bit\n";
            std::cout << "  Memory addressing: byte-addressed\n";
            std::cout << "  Interrupt signals: 1 byte\n";
            std::cout << "  Instruction types: R-type, I-type\n";
            std::cout << "  Opcode width:      8 bits\n";
            std::cout << "  Endianness:        little-endian\n";
            std::cout << "\n  Interrupts:\n";
            std::cout << "   - 0x00  Non-maskable interrupt\n";
            std::cout << "   - 0x01  Timer\n";
            std::cout << "   - 0x02  Keyboard\n";
            std::cout << "   - 0x03  External\n";
            std::cout << "   - 0x04  Software\n";
            std::cout << "   - 0x05  Division by zero\n";
            std::cout << "   - 0x06  Illegal instruction\n";
            std::cout << "   - 0x07  Memory fault\n";
            std::cout << "   - 0x08  Alignment fault\n";
            std::cout << "   - 0x09  Stack fault\n";
        } else {
            auto it = instructionList.find(written);

            if (it == instructionList.end()) {
                std::cout << "Invalid instruction name\n";
            } else {
                Instruction& instruction = it->second;

                std::cout << "Name: " << written << '\n';
                std::cout << "Description: " << instruction.description << '\n';
                std::cout << "Type: ";
                switch (instruction.type) {
                    case R:
                        std::cout << "R-type\n";
                        break;
                    case I:
                        std::cout << "I-type\n";
                        break;
                    case None:
                        std::cout << "No type\n";
                        break;
                }
                std::cout << "Operand(s): ";
                for (size_t i = 0; i < instruction.operands.size(); i++) {
                    OperandType type = instruction.operands[i];
                    switch (type) {
                        case imm12:
                            std::cout << "12-bit Immediate";
                            break;
                        case imm16:
                            std::cout << "16-bit Immediate";
                            break;
                        case reg:
                            std::cout << "Register";
                            break;
                        case none:
                            break;
                    }
                    if (type != none && i != instruction.operands.size() - 1) {
                        std::cout << ", ";
                    } else if (i == instruction.operands.size() - 1) {
                        std::cout << '\n';
                    }
                }
            }
        }
    }
}