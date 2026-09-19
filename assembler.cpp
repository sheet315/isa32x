#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <cstdio>

enum Intermediary {
    String,
    Integer,
    None
};

enum Operand {
    Register,
    Immediate12,
    Immediate16
};

enum TokenType {
    r0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,

    INT_LITERAL,
    LABEL_LITERAL,
    LABEL,

    NOP,
    ADD,
    SUB,
    MUL,
    DIV,
    INC,
    DEC,
    NOT,
    OR,
    AND,
    XOR,
    LSH,
    RSH,
    MOV,
    LDHI,
    LDLI,
    PUSH,
    POP,
    LOAD,
    SLOAD,
    STORE,
    OUT,
    IN,
    CALL,
    RET,
    IRET,
    WFI,
    CMP,
    JMP,
    JZ,
    JNZ,
    JLU,
    JGU,
    JLS,
    JGS,
    EI,
    DI,
    ESI,
    DSI,
    FI,
    JMPR,
    JZR,
    JNZR,
    JLUR,
    JGUR,
    JLSR,
    JGSR,
    CALLR,
    SE,
    HOLD,
    BITCT,
    SWAP,

    AT,
    COLON,

    NONE
};

struct Token {
    std::string value;
    TokenType   type;

    Token(std::string value, TokenType type) : value(value), type(type) {}
    Token() {};
};

std::ifstream            file;
std::vector<Token>       tokens;
std::vector<std::string> labels;

std::unordered_map<std::string_view, TokenType> lookup = {
    {"r0",     r0},
    {"r1",     r1},
    {"r2",     r2},
    {"r3",     r3},
    {"r4",     r4},
    {"r5",     r5},
    {"r6",     r6},
    {"r7",     r7},
    {"r8",     r8},
    {"r9",     r9},
    {"r10",   r10},
    {"r11",   r11},
    {"r12",   r12},
    {"r13",   r13},
    {"r14",   r14},
    {"r15",   r15},
    {"nop",   NOP},
    {"add",   ADD},
    {"sub",   SUB},
    {"mul",   MUL},
    {"div",   DIV},
    {"inc",   INC},
    {"dec",   DEC},
    {"not",   NOT},
    {"or",    OR},
    {"and",   AND},
    {"xor",   XOR},
    {"lsh",   LSH},
    {"rsh",   RSH},
    {"mov",   MOV},
    {"ldhi",  LDHI},
    {"ldli",  LDLI},
    {"push",  PUSH},
    {"pop",   POP},
    {"load",  LOAD},
    {"sload", SLOAD},
    {"store", STORE},
    {"out",   OUT},
    {"in",    IN},
    {"call",  CALL},
    {"ret",   RET},
    {"iret",  IRET},
    {"wfi",   WFI},
    {"cmp",   CMP},
    {"jmp",   JMP},
    {"jz",    JZ},
    {"jnz",   JNZ},
    {"jlu",   JLU},
    {"jgu",   JGU},
    {"jls",   JLS},
    {"jgs",   JGS},
    {"ei",    EI},
    {"di",    DI},
    {"esi",   ESI},
    {"dsi",   DSI},
    {"fi",    FI},
    {"jmpr",  JMPR},
    {"jzr",   JZR},
    {"jnzr",  JNZR},
    {"jlur",  JLUR},
    {"jgur",  JGUR},
    {"jlsr",  JLSR},
    {"jgsr",  JGSR},
    {"callr", CALLR},
    {"se",    SE},
    {"hold",  HOLD},
    {"bitct", BITCT},
    {"swap",  SWAP}
};

TokenType find(std::string str) {
    auto it = lookup.find(str);

    if (it == lookup.end()) {
        return TokenType::NONE;
    }

    return it->second;
}

std::unordered_map<TokenType, std::vector<Operand>> opToOperands = {
    {NOP,   {}},
    {ADD,   {Register, Register, Register}},
    {SUB,   {Register, Register, Register}},
    {MUL,   {Register, Register, Register}},
    {DIV,   {Register, Register, Register}},
    {INC,   {Register}},
    {DEC,   {Register}},
    {NOT,   {Register}},
    {OR,    {Register, Register, Register}},
    {AND,   {Register, Register, Register}},
    {XOR,   {Register, Register, Register}},
    {LSH,   {Register, Register, Register}},
    {RSH,   {Register, Register, Register}},
    {MOV,   {Register, Register}},
    {LDHI,  {Register, Immediate16}},
    {LDLI,  {Register, Immediate16}},
    {PUSH,  {Register}},
    {POP,   {Register}},
    {LOAD,  {Register, Register, Register}},
    {SLOAD, {Register, Register, Register}},
    {STORE, {Register, Register, Register}},
    {OUT,   {Register, Register}},
    {IN,    {Register, Register}},
    {CALL,  {Immediate16}},
    {RET,   {}},
    {IRET,  {}},
    {WFI,   {}},
    {CMP,   {Register, Register}},
    {JMP,   {Immediate16}},
    {JZ,    {Immediate16}},
    {JNZ,   {Immediate16}},
    {JLU,   {Immediate16}},
    {JGU,   {Immediate16}},
    {JLS,   {Immediate16}},
    {JGS,   {Immediate16}},
    {EI,    {}},
    {DI,    {}},
    {ESI,   {Register}},
    {DSI,   {Register}},
    {FI,    {Register}},
    {JMPR,  {Register}},
    {JZR,   {Register}},
    {JNZR,  {Register}},
    {JLUR,  {Register}},
    {JGUR,  {Register}},
    {JLSR,  {Register}},
    {JGSR,  {Register}},
    {CALLR, {Register}},
    {SE,    {Register, Register, Register}},
    {HOLD,  {Register, Immediate16}},
    {BITCT, {Register, Register}},
    {SWAP,  {Register, Register}},

};

void prescanLabels() {
    char c;

    std::string  currentString;
    Intermediary intermediary = None;

    while (file.get(c)) {
        if (isalpha(c) || c == '_' || (intermediary != None && isdigit(c))) {
            currentString += c;
            if (intermediary == None) {
                intermediary = isdigit(c) ? Integer : String;
            }
        } else if (c == ':') {
            if (intermediary == String) {
                labels.push_back(currentString);
            }
            currentString = "";
            intermediary = None;
        } else {
            currentString = "";
            intermediary = None;
        }
    }

    file.clear();
    file.seekg(0, std::ios::beg);
}

void tokenize() {
    char c;

    std::string  currentString;
    Intermediary intermediary = None;

    while (file.get(c)) {
        if (isalpha(c) || c == '_') {
            switch (intermediary) {
                case String: {
                    currentString += c;
                    break;
                }
                case Integer: {
                    throw std::runtime_error("cannot have alphanumerical or '_' after an integer literal."); 
                    break;
                }
                case None: {
                    currentString += c;
                    intermediary = String;
                    break;
                }
            }
        } else if (isdigit(c)) {
            switch (intermediary) {
                case String: {
                    currentString += c;
                    break;
                }
                case Integer: {
                    currentString += c;
                    break;
                }
                case None: {
                    currentString += c;
                    intermediary = Integer;
                    break;
                }
            }
        } else if (c == ' ' || c == ',' || c == '\n') {
            switch (intermediary) {
                case String: {
                    TokenType type = find(currentString);
                    if (type != NONE) {
                        tokens.emplace_back(currentString, type);
                    } else {
                        bool isValid = false;
                        for (auto l : labels) {
                            if (currentString == l) {
                                tokens.emplace_back(currentString, LABEL_LITERAL);
                                isValid = true;
                            }
                        }
                        if (!isValid) throw std::runtime_error("invalid keyword: " + currentString + '.');
                    }
                    currentString = "";
                    intermediary = None;
                    break;
                }
                case Integer: {
                    tokens.emplace_back(currentString, INT_LITERAL);
                    currentString = "";
                    intermediary = None;
                    break;
                }
                case None: {
                    currentString = "";
                    intermediary = None;
                    break;
                }
            }
        } else if (c == ':') {
            switch (intermediary) {
                case String: {
                    tokens.emplace_back(currentString, LABEL);
                    currentString = "";
                    intermediary = None;
                    break;
                }
                case Integer: {
                    tokens.emplace_back(currentString, INT_LITERAL);
                    tokens.emplace_back(":", COLON);
                    currentString = "";
                    intermediary = None;
                    break;
                }
                case None: {
                    tokens.emplace_back(":", COLON);
                    break;
                }
            }
        } else if (c == '@') {
            switch (intermediary) {
                case String: {
                    TokenType type = find(currentString);
                    if (type != NONE) {
                        tokens.emplace_back(currentString, type);
                    } else {
                        bool isValid = false;
                        for (auto l : labels) {
                            if (currentString == l) {
                                tokens.emplace_back(currentString, LABEL_LITERAL);
                                isValid = true;
                            }
                        }
                        if (!isValid) throw std::runtime_error("invalid keyword: " + currentString + '.');
                    }
                    tokens.emplace_back("@", AT);
                    currentString = "";
                    intermediary = None;
                    break;
                }
                case Integer: {
                    tokens.emplace_back(currentString, INT_LITERAL);
                    tokens.emplace_back("@", AT);
                    currentString = "";
                    intermediary = None;
                    break;
                }
                case None: {
                    throw std::runtime_error("");
                    currentString = "";
                    intermediary = None;
                    break;
                }
            }
        }
    }
    switch (intermediary) {
        case String: {
            TokenType type = find(currentString);
            if (type != NONE) {
                tokens.emplace_back(currentString, type);
            } else {
                bool isValid = false;
                for (auto l : labels) {
                    if (currentString == l) {
                        tokens.emplace_back(currentString, LABEL_LITERAL);
                        isValid = true;
                    }
                }
                if (!isValid) throw std::runtime_error("invalid keyword: " + currentString + '.');
            }
            currentString = "";
            intermediary = None;
            break;
        }
        case Integer: {
            tokens.emplace_back(currentString, INT_LITERAL);
            currentString = "";
            intermediary = None;
            break;
        }
        case None: {
            currentString = "";
            intermediary = None;
            break;
        }
    }
}

uint32_t resolveImmediate(size_t& index, std::unordered_map<std::string, uint32_t>& labelAddresses, bool resolving) {
    if (index >= tokens.size()) throw std::runtime_error("expected immediate operand");

    Token& base = tokens[index];
    uint32_t value = 0;

    if (base.type == INT_LITERAL) {
        value = (uint32_t)std::stoul(base.value, nullptr, 0);
    } else if (base.type == LABEL_LITERAL) {
        if (resolving) {
            auto it = labelAddresses.find(base.value);
            if (it == labelAddresses.end()) throw std::runtime_error("undefined label: " + base.value);
            value = it->second;
        }
    } else {
        throw std::runtime_error("expected integer or label literal, got: " + base.value);
    }
    index++;

    if (index < tokens.size() && tokens[index].type == AT) {
        index++;
        if (index >= tokens.size()) throw std::runtime_error("expected slice specifier after '@'");

        if (tokens[index].type == COLON) {
            index++;
            if (index >= tokens.size() || tokens[index].type != INT_LITERAL) {
                throw std::runtime_error("expected bit width after '@:'");
            }
            int n = std::stoi(tokens[index].value);
            index++;
            value = (n >= 32) ? value : ((value >> (32 - n)) & ((1u << n) - 1));
        } else if (tokens[index].type == INT_LITERAL) {
            int n = std::stoi(tokens[index].value);
            index++;
            if (index >= tokens.size() || tokens[index].type != COLON) {
                throw std::runtime_error("expected ':' after bit width in '@N:'");
            }
            index++;
            value = (n >= 32) ? value : (value & ((1u << n) - 1));
        } else {
            throw std::runtime_error("invalid slice specifier after '@'");
        }
    }

    return value;
}

size_t consumeInstruction(size_t index, std::vector<uint8_t>* out, std::unordered_map<std::string, uint32_t>& labelAddresses, bool resolving) {
    Token& opTok = tokens[index];
    TokenType opType = opTok.type;
    index++;

    auto it = opToOperands.find(opType);
    if (it == opToOperands.end()) throw std::runtime_error("unknown opcode: " + opTok.value);
    std::vector<Operand>& operands = it->second;

    uint8_t opcode = (uint8_t)(opType - NOP);

    bool isNoneType = operands.empty();
    bool isIType = false;
    for (Operand o : operands) {
        if (o == Immediate16 || o == Immediate12) isIType = true;
    }

    uint8_t  regs[3]  = {0, 0, 0};
    int      regCount = 0;
    uint32_t imm      = 0;

    for (Operand expected : operands) {
        if (expected == Register) {
            if (index >= tokens.size() || tokens[index].type < r0 || tokens[index].type > r15) {
                throw std::runtime_error("expected register operand for " + opTok.value);
            }
            if (regCount >= 3) throw std::runtime_error("too many register operands for " + opTok.value);
            regs[regCount++] = (uint8_t)(tokens[index].type - r0);
            index++;
        } else {
            int      bits  = (expected == Immediate12) ? 12 : 16;
            uint32_t value = resolveImmediate(index, labelAddresses, resolving);
            uint32_t mask  = (1u << bits) - 1;
            if (resolving && (value & ~mask) != 0) {
                throw std::runtime_error("immediate value out of range for " + opTok.value);
            }
            imm = value & mask;
        }
    }

    if (out != nullptr) {
        uint8_t b0, b1, b2, b3;
        if (isNoneType) {
            b0 = opcode;
            b1 = 0;
            b2 = 0;
            b3 = 0;
        } else if (isIType) {
            b0 = opcode;
            b1 = (uint8_t)(((regs[0] & 0xF) << 4) | (regs[1] & 0xF));
            b2 = (uint8_t)((imm >> 8) & 0xFF);
            b3 = (uint8_t)(imm & 0xFF);
        } else {
            b0 = opcode;
            b1 = (uint8_t)(((regs[0] & 0xF) << 4) | (regs[1] & 0xF));
            b2 = (uint8_t)(((regs[2] & 0xF) << 4) | ((imm >> 8) & 0xF));
            b3 = (uint8_t)(imm & 0xFF);
        }
        out->push_back(b0);
        out->push_back(b1);
        out->push_back(b2);
        out->push_back(b3);
    }

    return index;
}

std::vector<uint8_t> parse() {
    std::vector<uint8_t> output;
    std::unordered_map<std::string, uint32_t> labelAddresses;

    {
        size_t   index      = 0;
        uint32_t instrCount = 0;
        while (index < tokens.size()) {
            Token& t = tokens[index];
            if (t.type == LABEL) {
                labelAddresses[t.value] = instrCount * 4;
                index++;
            } else if (t.type >= NOP && t.type <= SWAP) {
                index = consumeInstruction(index, nullptr, labelAddresses, false);
                instrCount++;
            } else {
                throw std::runtime_error("unexpected token: " + t.value);
            }
        }
    }

    {
        size_t index = 0;
        while (index < tokens.size()) {
            Token& t = tokens[index];
            if (t.type == LABEL) {
                index++;
            } else if (t.type >= NOP && t.type <= SWAP) {
                index = consumeInstruction(index, &output, labelAddresses, true);
            } else {
                throw std::runtime_error("unexpected token: " + t.value);
            }
        }
    }

    return output;
}

int main() {
    file.open("test.txt");
    prescanLabels();
    tokenize();

    std::vector<uint8_t> machineCode = parse();

    std::ofstream out("out.bin", std::ios::binary);
    out.write((const char*)machineCode.data(), (std::streamsize)machineCode.size());
    out.close();

    std::cout << "wrote " << machineCode.size() << " bytes to out.bin\n";
    for (size_t i = 0; i < machineCode.size(); i++) {
        printf("%02x ", machineCode[i]);
        if (i % 4 == 3) std::cout << '\n';
    }
}