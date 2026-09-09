#include "instructions/m_extension.h"
#include "cpu.h"
#include "log.h"
#include <stdio.h>

static void mul(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, rs1 * rs2);
}

static void mulh(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    int64_t rs2 = (int64_t)cpu_read_reg(cpu, instruction.rs2);

    __int128_t product = (__int128_t)rs1 * rs2;
    int64_t result = (int64_t)(product >> 64);

    cpu_write_reg(cpu, instruction.rd, result);
}

static void mulhsu(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    __int128_t product = (__int128_t)rs1 * rs2;
    int64_t result = (int64_t)(product >> 64);

    cpu_write_reg(cpu, instruction.rd, result);
}

static void mulhu(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    __uint128_t product = (__uint128_t)rs1 * rs2;
    uint64_t result = (uint64_t)(product >> 64);

    cpu_write_reg(cpu, instruction.rd, result);
}

static void div(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    int64_t rs2 = (int64_t)cpu_read_reg(cpu, instruction.rs2);
    
    int64_t result;
    if (rs1 == INT64_MIN && rs2 == -1)
        result = INT64_MIN;
    else
        result = rs2 == 0 ? 0xFFFFFFFFFFFFFFFF : rs1 / rs2;

    cpu_write_reg(cpu, instruction.rd, result);
}

static void divu(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    uint64_t result = rs2 == 0 ? 0xFFFFFFFFFFFFFFFF : rs1 / rs2;

    cpu_write_reg(cpu, instruction.rd, result);
}

static void rem(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    int64_t rs2 = (int64_t)cpu_read_reg(cpu, instruction.rs2);

    int64_t result;
    if (rs1 == INT64_MIN && rs2 == -1)
        result = 0;
    else
        result = rs2 == 0 ? rs1 : rs1 % rs2;

    cpu_write_reg(cpu, instruction.rd, result);
}

static void remu(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    int64_t result = rs2 == 0 ? rs1 : rs1 % rs2;

    cpu_write_reg(cpu, instruction.rd, result);
}

static void mulw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t rs1 = (int32_t)cpu_read_reg(cpu, instruction.rs1);
    int32_t rs2 = (int32_t)cpu_read_reg(cpu, instruction.rs2);

    int64_t product = rs1 * rs2;

    cpu_write_reg(cpu, instruction.rd, sign_extend((int32_t)product, 32));
}

static void divw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t rs1 = (int32_t)cpu_read_reg(cpu, instruction.rs1);
    int32_t rs2 = (int32_t)cpu_read_reg(cpu, instruction.rs2);

    int64_t result;
    if (rs1 == INT32_MIN && rs2 == -1)
        result = INT32_MIN;
    else
        result = rs2 == 0 ? 0xFFFFFFFFFFFFFFFF : rs1 / rs2;

    cpu_write_reg(cpu, instruction.rd, sign_extend((int32_t)result, 32));
}

static void divuw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint32_t rs1 = (uint32_t)cpu_read_reg(cpu, instruction.rs1);
    uint32_t rs2 = (uint32_t)cpu_read_reg(cpu, instruction.rs2);

    uint64_t result = rs2 == 0 ? 0xFFFFFFFF : rs1 / rs2;

    cpu_write_reg(cpu, instruction.rd, sign_extend(result, 32));
}

static void remw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t rs1 = (int32_t)cpu_read_reg(cpu, instruction.rs1);
    int32_t rs2 = (int32_t)cpu_read_reg(cpu, instruction.rs2);

    int64_t result;
    if (rs1 == INT32_MIN && rs2 == -1)
        result = 0;
    else
        result = rs2 == 0 ? rs1 : rs1 % rs2;

    cpu_write_reg(cpu, instruction.rd, sign_extend((int32_t)result, 32));
}

static void remuw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint32_t rs1 = (uint32_t)cpu_read_reg(cpu, instruction.rs1);
    uint32_t rs2 = (uint32_t)cpu_read_reg(cpu, instruction.rs2);

    uint64_t result = rs2 == 0 ? (uint32_t)rs1 : rs1 % rs2;

    cpu_write_reg(cpu, instruction.rd, sign_extend(result, 32));
}

void dispatch_m_op(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    switch(instruction.funct3) {
            case 0x0: mul(cpu, mem, instruction); return;
            case 0x1: mulh(cpu, mem, instruction); return;
            case 0x2: mulhsu(cpu, mem, instruction); return;
            case 0x3: mulhu(cpu, mem, instruction); return;
            case 0x4: div(cpu, mem, instruction); return;
            case 0x5: divu(cpu, mem, instruction); return;
            case 0x6: rem(cpu, mem, instruction); return;
            case 0x7: remu(cpu, mem, instruction); return;
    }

    log_error("Illegal instruction\n");
}

void dispatch_m_op_32(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    switch(instruction.funct3) {
            case 0x0: mulw(cpu, mem, instruction); return;
            case 0x4: divw(cpu, mem, instruction); return;
            case 0x5: divuw(cpu, mem, instruction); return;
            case 0x6: remw(cpu, mem, instruction); return;
            case 0x7: remuw(cpu, mem, instruction); return;
    }

    log_error("Illegal instruction\n");
}
