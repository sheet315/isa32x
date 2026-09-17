#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>

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
    std::string name;
    std::string description;
    Type type;
    std::vector<OperandType> operands;

    Instruction(std::string name, std::string description, Type type, std::vector<OperandType> operands) : name(name), description(description), type(type), operands(operands) {}
};

std::unordered_map<std::string, Instruction> instructionList = {
    {"nop",   Instruction("nop",   "no operation",                                                          None, {})},
    {"add",   Instruction("add",   "r1 = r2 + r3",                                                          R,    {reg, reg, reg})},
    {"sub",   Instruction("sub",   "r1 = r2 - r3",                                                          R,    {reg, reg, reg})},
    {"mul",   Instruction("mul",   "r1 = r2 * r3",                                                          R,    {reg, reg, reg})},
    {"div",   Instruction("div",   "r1 = r2 / r3",                                                          R,    {reg, reg, reg})},
    {"inc",   Instruction("inc",   "r1++",                                                                  R,    {reg})},
    {"dec",   Instruction("dec",   "r1--",                                                                  R,    {reg})},
    {"not",   Instruction("not",   "r1 = ~r1",                                                              R,    {reg})},
    {"or",    Instruction("or",    "r1 = r2 | r3",                                                          R,    {reg, reg, reg})},
    {"and",   Instruction("and",   "r1 = r2 & r3",                                                          R,    {reg, reg, reg})},
    {"xor",   Instruction("xor",   "r1 = r2 ^ r3",                                                          R,    {reg, reg, reg})},
    {"lsh",   Instruction("lsh",   "r1 = r2 << r3",                                                         R,    {reg, reg, reg})},
    {"rsh",   Instruction("rsh",   "r1 = r2 >> r3",                                                         R,    {reg, reg, reg})},
    {"mov",   Instruction("mov",   "r1 = r2",                                                               R,    {reg, reg})},
    {"ldhi",  Instruction("ldhi",  "r1[top 16] = imm16",                                                    I,    {reg, none, imm16})},
    {"ldli",  Instruction("ldli",  "r1[bottom 16] = imm16",                                                 I,    {reg, none, imm16})},
    {"push",  Instruction("push",  "stack[ptr++] = r1",                                                     R,    {reg})},
    {"pop",   Instruction("pop",   "stack r1 = stack[--ptr]",                                               R,    {reg})},
    {"load",  Instruction("load",  "r1 = mem[r2] unsigned, r3 byte count, max 4",                           R,    {reg, reg, reg})},
    {"sload", Instruction("sload", "r1 = mem[r2] signed, r3 byte count, max 4",                             R,    {reg, reg, reg})},
    {"store", Instruction("store", "mem[r1] = r2 unsigned / just copies bit pattern, r3 byte count, max 4", R,    {reg, reg, reg})},
    {"out",   Instruction("out",   "output[r1] = r2",                                                       R,    {reg, reg})},
    {"in",    Instruction("in",    "r1 = input[r2]",                                                        R,    {reg, reg})},
    {"call",  Instruction("call",  "pc = label, push addr",                                                 I,    {none, none, imm16})},
    {"ret",   Instruction("ret",   "pc = pop addr",                                                         None, {})},
    {"iret",  Instruction("iret",  "pop 16x registers sequentially, return to stored pc",                   None, {})},
    {"wfi",   Instruction("wfi",   "wait until interrupt",                                                  None, {})},
    {"cmp",   Instruction("cmp",   "compare r1 r2",                                                         R,    {reg, reg})},
    {"jmp",   Instruction("jmp",   "jump to label unconditionally",                                         I,    {none, none, imm16})},
    {"jz",    Instruction("jz",    "jump if zero",                                                          I,    {none, none, imm16})},
    {"jnz",   Instruction("jnz",   "jump if not zero",                                                      I,    {none, none, imm16})},
    {"jlu",   Instruction("jlu",   "jump less unsigned",                                                    I,    {none, none, imm16})},
    {"jgu",   Instruction("jgu",   "jump greater unsigned",                                                 I,    {none, none, imm16})},
    {"jls",   Instruction("jls",   "jump less signed",                                                      I,    {none, none, imm16})},
    {"jgs",   Instruction("jgs",   "jump greater signed",                                                   I,    {none, none, imm16})},
    {"ei",    Instruction("ei",    "enable interrupts",                                                     None, {})},
    {"di",    Instruction("di",    "disable interrupts",                                                    None, {})},
    {"esi",   Instruction("esi",   "enable interrupt id r1",                                                R,    {reg})},
    {"dsi",   Instruction("dsi",   "disable interrupt id r1",                                               R,    {reg})},
    {"fi",    Instruction("fi",    "force interrupt id r1",                                                 R,    {reg})},
    {"jmpr",  Instruction("jmpr",  "jump to r1 value unconditionally",                                      R,    {reg})},
    {"jzr",   Instruction("jzr",   "jump to r1 val if zero",                                                R,    {reg})},
    {"jnzr",  Instruction("jnzr",  "jump to r1 val if not zero",                                            R,    {reg})},
    {"jlur",  Instruction("jlur",  "jump to r1 val less unsigned",                                          R,    {reg})},
    {"jgur",  Instruction("jgur",  "jump to r1 val greater unsigned",                                       R,    {reg})},
    {"jlsr",  Instruction("jlsr",  "jump to r1 val less signed",                                            R,    {reg})},
    {"jgsr",  Instruction("jgsr",  "jump to r1 val greater signed",                                         R,    {reg})},
    {"callr", Instruction("callr", "jump to r1 val, push pc",                                               R,    {reg})},
    {"se",    Instruction("se",    "sign extend r1 = r2, r3 source bit count",                              R,    {reg, reg, reg})},
};

int main() {
    for (const auto& [name, instruction] : instructionList) {
        assert(name == instruction.name);
    }
}