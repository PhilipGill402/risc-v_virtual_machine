#ifndef INCLUDE_INSTRUCTIONS_DECODING_H_
#define INCLUDE_INSTRUCTIONS_DECODING_H_

#include <stdint.h>
#include "memory.h"

typedef enum Opcode {
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
} opcode_t;

typedef struct {
    uint8_t rd : 4;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t funct7 : 6;
    uint8_t opcode : 6;
} rtype_t;

typedef struct {
    uint8_t rd : 4;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} itype_t;

typedef struct {
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} stype_t;

typedef struct {
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} btype_t;

typedef struct {
    uint8_t rd : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} utype_t;

typedef struct {
    uint8_t rd : 4;
    uint8_t opcode : 6;
    uint64_t imm;
} jtype_t;

uint64_t sign_extend(uint32_t imm, uint8_t bits);

rtype_t decodeR(uint32_t instruction);
itype_t decodeI(uint32_t instruction);
stype_t decodeS(uint32_t instruction);
btype_t decodeB(uint32_t instruction);
utype_t decodeU(uint32_t instruction);
jtype_t decodeJ(uint32_t instruction);

#endif
