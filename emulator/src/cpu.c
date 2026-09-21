#include "cpu.h"
#include "instructions/decoding.h"
#include "instructions/itype.h"
#include "instructions/jtype.h"
#include "instructions/rtype.h"
#include "instructions/stype.h"
#include "instructions/utype.h"
#include "instructions/btype.h"
#include "csrs/csr_def.h"
#include "csrs/csr.h"
#include "log.h"
#include "trap.h"
#include "paging.h"
#include <stdio.h>
#include <string.h>

static inline uint8_t is_mret(uint32_t instruction) {
    return instruction == 0x30200073;
}

static void increment_pc(cpu_t* cpu, uint32_t instruction) {
    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode; 
    if (opcode != JAL && opcode != JALR && opcode != BRANCH && !cpu->trap_taken && !is_mret(instruction))
        cpu->pc += 4;
}

static void dispatch_instruction(cpu_t* cpu, memory_t* mem, uint32_t instruction) {
    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode;

    switch (opcode) {
        case LUI:
        case AUIPC: {
            utype_t decoded = decodeU(instruction);
            executeU(cpu, mem, decoded);
            break;
        }
        
        case JAL: {
            jtype_t decoded = decodeJ(instruction);
            executeJ(cpu, mem, decoded);
            break;
        }

        case JALR:
        case LOAD:
        case OP_IMM:
        case SYSTEM:
        case MISC_MEM:
        case OP_IMM_32: {
            itype_t decoded = decodeI(instruction);
            executeI(cpu, mem, decoded);
            break;
        }

        case BRANCH: {
            btype_t decoded = decodeB(instruction);
            executeB(cpu, mem, decoded);
            break;
        }

        case STORE: {
            stype_t decoded = decodeS(instruction);
            executeS(cpu, mem, decoded);
            break;
        }

        case OP:
        case OP_32: {
            rtype_t decoded = decodeR(instruction);
            executeR(cpu, mem, decoded);
            break;
        }

        default: {
            // illegal instruction
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        }
    }
}

static void cpu_check_interrupts(cpu_t* cpu) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];
    uint64_t mip = cpu->csrs[CSR_MIP];
    uint64_t mie = cpu->csrs[CSR_MIE];
    
    uint8_t global_mie = (uint8_t)(mstatus >> 3) & 0x1; // are interrupts enabled?
    
    if (!global_mie)
        return;

    uint64_t pending = mip & mie; // checks if an interrupt is pending and it is enabled

    if (!pending)
        return;

    if (pending & (1ULL << 11)) { // machine external interrupt
        
    } else if (pending & (1ULL << 3)) { // machine software interrupt
    
    } else if (pending & (1ULL << 7)) { //machine timer interrupt
        log_debug("Timer interrupt generated\n");
        raise_interrupt(cpu, 0x7);
    }
}

cpu_t cpu_init() {
    cpu_t cpu = { 0 };

    csr_load(&cpu);
    
    return cpu;
}

void cpu_reset(cpu_t* cpu) {
    cpu->pc = MEM_BASE;
    cpu->priviledge = M_MODE;
    cpu->trap_taken = 0;
    
    memset(cpu->regs, 0, sizeof(cpu->regs));
    csr_reset(cpu);
}

static fetch_result_t cpu_fetch(cpu_t* cpu, memory_t* mem) {
    translation_result_t result = translate_address(cpu, mem, cpu->pc, ACCESS_FETCH);
    fetch_result_t fetch_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, cpu->pc);
        return fetch_result;
    }
    
    fetch_result.success = 1;
    fetch_result.value = mem_read32(mem, result.physical_address);
    return fetch_result;
}

void cpu_write_reg(cpu_t* cpu, uint8_t reg_num, uint64_t value) {
    if (reg_num == 0)
        return;
    
    if (reg_num >= 32) {
        log_error("Register number out of range: %u\n", reg_num);
        return;
    }

    cpu->regs[reg_num] = value;
}

uint64_t cpu_read_reg(cpu_t* cpu, uint8_t reg_num) {
    if (reg_num == 0)
        return 0;

    if (reg_num >= 32) {
        log_error("Register number out of range: %u\n", reg_num);
        return 0;
    }

    return cpu->regs[reg_num];
}

void cpu_step(cpu_t* cpu, memory_t* mem) {
    fetch_result_t result = cpu_fetch(cpu, mem);
    
    if (!result.success)
        return;

    uint32_t instruction = result.value;
    dispatch_instruction(cpu, mem, instruction);
    
    cpu_check_interrupts(cpu);
    
    increment_pc(cpu, instruction);
    cpu->trap_taken = 0;
}

void cpu_set_interrupt_pending(cpu_t* cpu, uint8_t cause, uint8_t pending) {
    if (pending)
        cpu->csrs[CSR_MIP] |= 1ULL << cause;
    else
        cpu->csrs[CSR_MIP] &= ~(1ULL << cause);
}

load_result_t cpu_load8(cpu_t* cpu, memory_t* mem, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 }; 

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }

    load_result.success = 1;
    load_result.value = mem_read8(mem, result.physical_address);
    return load_result;
}

load_result_t cpu_load16(cpu_t* cpu, memory_t* mem, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }
        
    load_result.success = 1;
    load_result.value = mem_read16(mem, result.physical_address);
    return load_result;
}

load_result_t cpu_load32(cpu_t* cpu, memory_t* mem, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }

    load_result.success = 1;
    load_result.value = mem_read32(mem, result.physical_address);
    return load_result;
}

load_result_t cpu_load64(cpu_t* cpu, memory_t* mem, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }
    
    load_result.success = 1;
    load_result.value = mem_read64(mem, result.physical_address);
    return load_result;
}

store_result_t cpu_store8(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint8_t value) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }
    
    mem_write8(mem, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

store_result_t cpu_store16(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint16_t value) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }

    mem_write16(mem, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

store_result_t cpu_store32(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint32_t value) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }

    mem_write32(mem, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

store_result_t cpu_store64(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint64_t value) {
    translation_result_t result = translate_address(cpu, mem, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }

    mem_write64(mem, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

