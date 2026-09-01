#ifndef INCLUDE_INSTRUCTION_TYPES_H_
#define INCLUDE_INSTRUCTION_TYPES_H_

#include <stdint.h>

typedef struct {
    uint8_t rd : 4;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
    uint8_t funct7 : 6;
} RType;

typedef struct {
    uint8_t rd : 4;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint16_t imm : 11; 
} IType;

typedef struct {
    uint16_t imm : 11;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
} SType;

typedef struct {
    uint16_t imm : 12;
    uint8_t funct3 : 2;
    uint8_t rs1 : 4;
    uint8_t rs2 : 4;
} BType;

typedef struct {
    uint8_t rd : 4;
    uint32_t imm : 19;
} UType;

typedef struct {
    uint8_t rd : 4;
    uint32_t imm : 20;
} JType;

RType decodeR(uint32_t instruction);
IType decodeI(uint32_t instruction);
SType decodeS(uint32_t instruction);
BType decodeB(uint32_t instruction);
UType decodeU(uint32_t instruction);
JType decodeJ(uint32_t instruction);

#endif
