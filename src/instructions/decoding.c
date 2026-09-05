#include "instructions/decoding.h"

#include <stdio.h>

uint64_t sign_extend(uint32_t imm, uint8_t bits) {
    uint8_t sign = (imm & (1 << (bits - 1))) != 0;
    uint64_t result = 0;

    if (sign)
        result = (0xFFFFFFFFFFFFFFFFULL << bits) | imm;
    else
        result = (uint64_t)imm;
        
    return result;
}

rtype_t decodeR(uint32_t instruction) {
    return (rtype_t){
        .rd = (uint8_t)((instruction >> 7) & 0x1F),
        .funct3 = (uint8_t)((instruction >> 12) & 0x07),
        .rs1 = (uint8_t)((instruction >> 15) & 0x1F),
        .rs2 = (uint8_t)((instruction >> 20) & 0x1F),
        .funct7 = (uint8_t)((instruction >> 25) & 0x7F),
        .opcode = (uint8_t)instruction & 0x7f,
    };
}

itype_t decodeI(uint32_t instruction) {
    uint32_t imm = (uint32_t)(instruction >> 20 & 0xFFF);

    return (itype_t){
        .rd = (uint8_t)(instruction >> 7 & 0x1F),
        .funct3 = (uint8_t)(instruction >> 12 & 0x07),
        .rs1 = (uint8_t)(instruction >> 15 & 0x1F),
        .opcode = (uint8_t)instruction & 0x7f,
        .imm = sign_extend(imm, 12),
    };
}

stype_t decodeS(uint32_t instruction) {
    uint32_t imm = (uint32_t)(
            ((uint16_t)((instruction >> 7) & 0x1F)) |
            ((uint16_t)((instruction >> 25) & 0x7F) << 5));
    
    return (stype_t){
        .funct3 = (uint8_t)((instruction >> 12) & 0x07),
        .rs1 = (uint8_t)((instruction >> 15) & 0x1F),
        .rs2 = (uint8_t)((instruction >> 20) & 0x1F),
        .opcode = (uint8_t)instruction & 0x7f,
        .imm = sign_extend(imm, 12),
    };
}

btype_t decodeB(uint32_t instruction) {
    uint32_t imm = (uint32_t)(
                ((uint16_t)((instruction >> 8) & 0xF) << 1) |
                ((uint16_t)((instruction >> 25) & 0x3F) << 5) |
                ((uint16_t)((instruction >> 7) & 0x1) << 11) |
                ((uint16_t)((instruction >> 31) & 0x1) << 12));

    return (btype_t){
        .funct3 = (uint8_t)((instruction >> 12) & 0x7),
        .rs1 = (uint8_t)((instruction >> 15) & 0x1F),
        .rs2 = (uint8_t)((instruction >> 20) & 0x1F),
        .opcode = (uint8_t)instruction & 0x7f,
        .imm = sign_extend(imm, 13),
    };
}

utype_t decodeU(uint32_t instruction) {
    return (utype_t){
        .rd = (uint8_t)((instruction >> 7) & 0x1F),
        .opcode = (uint8_t)instruction & 0x7f,
        .imm = sign_extend(instruction & 0xFFFFF000, 32),
    };
}

jtype_t decodeJ(uint32_t instruction) {
    uint32_t imm = (uint32_t)(
            ((uint32_t)((instruction >> 21) & 0x3FF) << 1) |
            ((uint32_t)((instruction >> 20) & 0x1) << 11) |
            ((uint32_t)((instruction >> 12) & 0xFF) << 12) |
            ((uint32_t)((instruction >> 31) & 0x1) << 20));

    return (jtype_t){
        .rd = (uint8_t)((instruction >> 7) & 0x1F),
        .opcode = (uint8_t)(instruction & 0x7f),
        .imm =  sign_extend(imm, 21),
    };
}

