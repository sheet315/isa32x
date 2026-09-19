#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

enum Intermediary {
    String,
    Integer,
    None
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

    NONE
};

struct Token {
    std::string value;
    TokenType   type;

    Token(std::string value, TokenType type) : value(value), type(type) {}
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
                    throw std::runtime_error("cannot have integer as label");
                    break;
                }
                case None: {
                    throw std::runtime_error("label requires string");
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


int main() {
    file.open("test.txt");
    tokenize();
    for (auto t : tokens) {
        std::cout << t.value << ':' << (int)t.type << '\n';
    }
}