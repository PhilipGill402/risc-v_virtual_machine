#include "disassembler.h"
#include "tables.h"
#include "instructions/decoding.h"
#include <stdlib.h>
#include <stdio.h>

char* disassemble_line(memory_t* mem, uint64_t addr) {
    char* buffer = malloc(BUFFER_SIZE);
    if (!buffer)
        return NULL;

    uint32_t instruction = mem_read32(mem, addr);
    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode;

    snprintf(buffer, BUFFER_SIZE, ".word 0x%08x\n", instruction);

    switch (opcode) {
        case LUI: {
            utype_t decoded = decodeU(instruction);
            uint32_t imm20 = instruction >> 12;
            snprintf(buffer, BUFFER_SIZE, "lui x%u, 0x%05x\n", decoded.rd, imm20);
            break;
        }
        
        case AUIPC: {
            utype_t decoded = decodeU(instruction);
            uint32_t imm20 = instruction >> 12;
            snprintf(buffer, BUFFER_SIZE, "auipc x%u, 0x%012x\n", decoded.rd, imm20);
            break;
        }
        
        case JAL: {
            jtype_t decoded = decodeJ(instruction);
            snprintf(buffer, BUFFER_SIZE, "jal x%u, %lld\n", decoded.rd, (int64_t)decoded.imm);
            break;
        }

        case JALR: {
            itype_t decoded = decodeI(instruction);
            snprintf(buffer, BUFFER_SIZE, "jalr x%u, %lld(x%u)\n", decoded.rd, (int64_t)decoded.imm, decoded.rs1);
            break;
        }

        case LOAD: {
            itype_t decoded = decodeI(instruction);
            snprintf(buffer, BUFFER_SIZE, "%s x%u, %lld(x%u)\n", load_mnemonics[decoded.funct3], decoded.rd, (int64_t)decoded.imm, decoded.rs1);
            break;
        }

        case OP_IMM: {
            itype_t decoded = decodeI(instruction);
            uint8_t funct6 = (uint8_t)((instruction >> 26) & 0x3F);
            uint8_t shamt  = (uint8_t)((instruction >> 20) & 0x3F);
            
            if (decoded.funct3 == 0x1)
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %u\n", op_imm_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, shamt);
            else if (decoded.funct3 == 0x5) {
                if (funct6 == 0x00)
                    snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %u\n", op_imm_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, shamt);
                else if (funct6 == 0x10)
                    snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %u\n", op_imm_alt_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, shamt);
            } else {
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %lld\n", op_imm_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, (int64_t)decoded.imm);
            }

            break;        
        }

        case SYSTEM: {
            if (instruction == 0x00000073)
                snprintf(buffer, BUFFER_SIZE, "ecall\n");
            else if (instruction == 0x00100073)
                snprintf(buffer, BUFFER_SIZE, "ebreak\n");
            break;
        }

        case MISC_MEM: {
            itype_t decoded = decodeI(instruction);
            snprintf(buffer, BUFFER_SIZE, "%s\n", misc_mem_mnemonics[decoded.funct3]);
            break;
        }

        case OP_IMM_32: {
            itype_t decoded = decodeI(instruction);
            uint8_t funct7 = (uint8_t)((instruction >> 25) & 0x7F);
            uint8_t shamt  = (uint8_t)((instruction >> 20) & 0x1F);
            
            if (decoded.funct3 == 0x1)
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %u\n", op_imm_32_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, shamt);
            else if (decoded.funct3 == 0x5) {
                if (funct7 == 0x00)
                    snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %u\n", op_imm_32_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, shamt);
                else if (funct7 == 0x10)
                    snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %u\n", op_imm_32_alt_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, shamt);
            } else {
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, %d\n", op_imm_32_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, (int32_t)decoded.imm);
            }

            break;
        }

        case BRANCH: {
            btype_t decoded = decodeB(instruction);
            snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, 0x%05llx\n", branch_mnemonics[decoded.funct3], decoded.rs1, decoded.rs2, addr + (int64_t)decoded.imm);
            break;
        }

        case STORE: {
            stype_t decoded = decodeS(instruction);
            snprintf(buffer, BUFFER_SIZE, "%s x%u, %lld(x%u)", store_mnemonics[decoded.funct3], decoded.rs2, (int64_t)decoded.imm, decoded.rs1);
            break;
        }

        case OP: {
            rtype_t decoded = decodeR(instruction);
            
            if (decoded.funct7 == 0x0)
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, x%u\n", op_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, decoded.rs2);
            else
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, x%u\n", op_alt_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, decoded.rs2);

            break;
        }

        case OP_32: {
            rtype_t decoded = decodeR(instruction);
            
            if (decoded.funct7 == 0x0)
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, x%u\n", op32_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, decoded.rs2);
            else
                snprintf(buffer, BUFFER_SIZE, "%s x%u, x%u, x%u\n", op32_alt_mnemonics[decoded.funct3], decoded.rd, decoded.rs1, decoded.rs2);
            
            break;
        }

        default: {
        // illegal instruction
            fprintf(stderr, "Illegal instruction\n"); 
        }
    }
    
    return buffer;
}
