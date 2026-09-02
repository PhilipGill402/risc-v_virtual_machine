#ifndef INCLUDE_INSTRUCTIONS_DECODING_H_
#define INCLUDE_INSTRUCTIONS_DECODING_H_

#include <stdint.h>

enum class Opcode : uint8_t {
    LOAD       = 0x03,
    MISC_MEM   = 0x0F,
    OP_IMM     = 0x13,
    AUIPC      = 0x17,
    OP_IMM_32  = 0x1B,
    STORE      = 0x23,
    OP         = 0x33,
    LUI        = 0x37,
    OP_32      = 0x3B,
    BRANCH     = 0x63,
    JALR       = 0x67,
    JAL        = 0x6F,
    SYSTEM     = 0x73
};

typedef struct {
    uint8_t rd : 4;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t funct7 : 6;
    uint8_t opcode : 6;
} RType;

typedef struct {
    uint8_t rd : 4;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} IType;

typedef struct {
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} SType;

typedef struct {
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} BType;

typedef struct {
    uint8_t rd : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} UType;

typedef struct {
    uint8_t rd : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} JType;

uint64_t sign_extend(uint32_t imm, uint8_t bits);

RType decodeR(uint32_t instruction);
IType decodeI(uint32_t instruction);
SType decodeS(uint32_t instruction);
BType decodeB(uint32_t instruction);
UType decodeU(uint32_t instruction);
JType decodeJ(uint32_t instruction);

#endif
