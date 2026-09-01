#include "instruction_types.h"

#include <stdio.h>

// TODO: add sign extension

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
    return (IType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x07),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .imm = static_cast<uint16_t>((instruction >> 20) & 0xFFF),
    };
}

SType decodeS(uint32_t instruction) {
    return (SType){
        .imm = static_cast<uint16_t>(static_cast<uint16_t>((instruction >> 7) & 0x1F) | (static_cast<uint16_t>((instruction >> 25) & 0x7F) << 5)),
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x07),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .rs2 = static_cast<uint8_t>((instruction >> 20) & 0x1F),
    };
}

BType decodeB(uint32_t instruction) {
    return (BType){
        .imm = static_cast<uint16_t>(
                (static_cast<uint16_t>((instruction >> 8) & 0xF) << 1) |
                (static_cast<uint16_t>((instruction >> 25) & 0x3F) << 5) |
                (static_cast<uint16_t>((instruction >> 7) & 0x1) << 11) |
                (static_cast<uint16_t>((instruction >> 31) & 0x1) << 12)
                ),
        .funct3 = static_cast<uint8_t>((instruction >> 12) & 0x7),
        .rs1 = static_cast<uint8_t>((instruction >> 15) & 0x1F),
        .rs2 = static_cast<uint8_t>((instruction >> 20) & 0x1F),
    };
}

UType decodeU(uint32_t instruction) {
    return (UType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .imm = instruction & 0xFFFFF000,
    };
}

JType decodeJ(uint32_t instruction) {
    return (JType){
        .rd = static_cast<uint8_t>((instruction >> 7) & 0x1F),
        .imm =  (static_cast<uint32_t>((instruction >> 21) & 0x3FF) << 1) |
                (static_cast<uint32_t>((instruction >> 20) & 0x1) << 11) |
                (static_cast<uint32_t>((instruction >> 12) & 0xFF) << 12) |
                (static_cast<uint32_t>((instruction >> 31) & 0x1) << 20),
    };
}

