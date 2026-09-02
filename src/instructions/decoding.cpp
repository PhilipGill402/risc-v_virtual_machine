#include "instructions/decoding.h"

#include <stdio.h>

uint64_t sign_extend(uint32_t imm, uint8_t bits) {
    uint8_t sign = (imm & (1 << (bits - 1))) != 0;
    uint64_t result = 0;

    if (sign)
        result = (0xFFFFFFFFFFFFFFFFULL << bits) | imm;
    else
        result = static_cast<uint64_t>(imm);
        
    return result;
}

RType decodeR(uint32_t instruction) {
    return (RType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x07),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .rs2 = static_cast<uint8_t>((instruction >> 20) & 0x1F),
        .funct7 = static_cast<uint8_t>((instruction >> 25) & 0x7F),
    };
}

IType decodeI(uint32_t instruction) {
    uint32_t imm = static_cast<uint32_t>((instruction >> 20) & 0xFFF);

    return (IType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x07),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .imm = sign_extend(imm, 12),
    };
}

SType decodeS(uint32_t instruction) {
    uint32_t imm = static_cast<uint32_t>(
            (static_cast<uint16_t>((instruction >> 7) & 0x1F)) |
            (static_cast<uint16_t>((instruction >> 25) & 0x7F) << 5));
    
    return (SType){
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x07),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .rs2 = static_cast<uint8_t>((instruction >> 20) & 0x1F),
        .imm = sign_extend(imm, 12),
    };
}

BType decodeB(uint32_t instruction) {
    uint32_t imm = static_cast<uint32_t>(
                (static_cast<uint16_t>((instruction >> 8) & 0xF) << 1) |
                (static_cast<uint16_t>((instruction >> 25) & 0x3F) << 5) |
                (static_cast<uint16_t>((instruction >> 7) & 0x1) << 11) |
                (static_cast<uint16_t>((instruction >> 31) & 0x1) << 12));

    return (BType){
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x7),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .rs2 = static_cast<uint8_t>((instruction >> 20) & 0x1F),
        .imm = sign_extend(imm, 13),
    };
}

UType decodeU(uint32_t instruction) {
    return (UType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .imm = sign_extend(instruction & 0xFFFFF000, 32),
    };
}

JType decodeJ(uint32_t instruction) {
    uint32_t imm = static_cast<uint32_t>(
            (static_cast<uint32_t>((instruction >> 21) & 0x3FF) << 1) |
            (static_cast<uint32_t>((instruction >> 20) & 0x1) << 11) |
            (static_cast<uint32_t>((instruction >> 12) & 0xFF) << 12) |
            (static_cast<uint32_t>((instruction >> 31) & 0x1) << 20));

    return (JType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .imm =  sign_extend(imm, 21),
    };
}

