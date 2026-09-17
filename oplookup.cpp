#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <iostream>

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

    Instruction(std::string description, Type type, std::vector<OperandType> operands) : description(description), type(type), operands(operands) {}
};

std::unordered_map<std::string_view, Instruction> instructionList = {
    {"nop",   Instruction("no operation",                                                          None, {})},
    {"add",   Instruction("r1 = r2 + r3",                                                          R,    {reg, reg, reg})},
    {"sub",   Instruction("r1 = r2 - r3",                                                          R,    {reg, reg, reg})},
    {"mul",   Instruction("r1 = r2 * r3",                                                          R,    {reg, reg, reg})},
    {"div",   Instruction("r1 = r2 / r3",                                                          R,    {reg, reg, reg})},
    {"inc",   Instruction("r1++",                                                                  R,    {reg})},
    {"dec",   Instruction("r1--",                                                                  R,    {reg})},
    {"not",   Instruction("r1 = ~r1",                                                              R,    {reg})},
    {"or",    Instruction("r1 = r2 | r3",                                                          R,    {reg, reg, reg})},
    {"and",   Instruction("r1 = r2 & r3",                                                          R,    {reg, reg, reg})},
    {"xor",   Instruction("r1 = r2 ^ r3",                                                          R,    {reg, reg, reg})},
    {"lsh",   Instruction("r1 = r2 << r3",                                                         R,    {reg, reg, reg})},
    {"rsh",   Instruction("r1 = r2 >> r3",                                                         R,    {reg, reg, reg})},
    {"mov",   Instruction("r1 = r2",                                                               R,    {reg, reg})},
    {"ldhi",  Instruction("r1[top 16] = imm16",                                                    I,    {reg, none, imm16})},
    {"ldli",  Instruction("r1[bottom 16] = imm16",                                                 I,    {reg, none, imm16})},
    {"push",  Instruction("stack[ptr++] = r1",                                                     R,    {reg})},
    {"pop",   Instruction("stack r1 = stack[--ptr]",                                               R,    {reg})},
    {"load",  Instruction("r1 = mem[r2] unsigned, r3 byte count, max 4",                           R,    {reg, reg, reg})},
    {"sload", Instruction("r1 = mem[r2] signed, r3 byte count, max 4",                             R,    {reg, reg, reg})},
    {"store", Instruction("mem[r1] = r2 unsigned / just copies bit pattern, r3 byte count, max 4", R,    {reg, reg, reg})},
    {"out",   Instruction("output[r1] = r2",                                                       R,    {reg, reg})},
    {"in",    Instruction("r1 = input[r2]",                                                        R,    {reg, reg})},
    {"call",  Instruction("pc = label, push addr",                                                 I,    {none, none, imm16})},
    {"ret",   Instruction("pc = pop addr",                                                         None, {})},
    {"iret",  Instruction("pop 16x registers sequentially, return to stored pc",                   None, {})},
    {"wfi",   Instruction("wait until interrupt",                                                  None, {})},
    {"cmp",   Instruction("compare r1 r2",                                                         R,    {reg, reg})},
    {"jmp",   Instruction("jump to label unconditionally",                                         I,    {none, none, imm16})},
    {"jz",    Instruction("jump if zero",                                                          I,    {none, none, imm16})},
    {"jnz",   Instruction("jump if not zero",                                                      I,    {none, none, imm16})},
    {"jlu",   Instruction("jump less unsigned",                                                    I,    {none, none, imm16})},
    {"jgu",   Instruction("jump greater unsigned",                                                 I,    {none, none, imm16})},
    {"jls",   Instruction("jump less signed",                                                      I,    {none, none, imm16})},
    {"jgs",   Instruction("jump greater signed",                                                   I,    {none, none, imm16})},
    {"ei",    Instruction("enable interrupts",                                                     None, {})},
    {"di",    Instruction("disable interrupts",                                                    None, {})},
    {"esi",   Instruction("enable interrupt id r1",                                                R,    {reg})},
    {"dsi",   Instruction("disable interrupt id r1",                                               R,    {reg})},
    {"fi",    Instruction("force interrupt id r1",                                                 R,    {reg})},
    {"jmpr",  Instruction("jump to r1 value unconditionally",                                      R,    {reg})},
    {"jzr",   Instruction("jump to r1 val if zero",                                                R,    {reg})},
    {"jnzr",  Instruction("jump to r1 val if not zero",                                            R,    {reg})},
    {"jlur",  Instruction("jump to r1 val less unsigned",                                          R,    {reg})},
    {"jgur",  Instruction("jump to r1 val greater unsigned",                                       R,    {reg})},
    {"jlsr",  Instruction("jump to r1 val less signed",                                            R,    {reg})},
    {"jgsr",  Instruction("jump to r1 val greater signed",                                         R,    {reg})},
    {"callr", Instruction("jump to r1 val, push pc",                                               R,    {reg})},
    {"se",    Instruction("sign extend r1 = r2, r3 source bit count",                              R,    {reg, reg, reg})},
};

int main() {
    std::cout << "Enter an opcode to see its information.\nEnter 'list' to see a list of the opcodes.\nEnter 'quit' to exit program\n";
    
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