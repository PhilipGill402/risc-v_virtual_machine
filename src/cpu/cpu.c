#include "cpu/cpu.h"
#include "cpu/instructions/decoding.h"
#include "cpu/instructions/atype.h"
#include "cpu/instructions/itype.h"
#include "cpu/instructions/jtype.h"
#include "cpu/instructions/rtype.h"
#include "cpu/instructions/stype.h"
#include "cpu/instructions/utype.h"
#include "cpu/instructions/btype.h"
#include "cpu/csrs/csr_def.h"
#include "cpu/csrs/csr.h"
#include "cpu/trap.h"
#include "cpu/paging.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

void cpu_invalidate_reservation(cpu_t* cpu, uint64_t phys_addr, uint8_t size) {
    if (!cpu->reservation.valid)
        return;

    uint64_t store_start = phys_addr;
    uint64_t store_end = phys_addr + size;

    uint64_t resrvation_start = cpu->reservation.phys_addr;
    uint64_t resrvation_end = resrvation_start + cpu->reservation.size;
    
    // the store falls inside of the reservation
    if (store_start < resrvation_end && store_end > resrvation_start)
        cpu->reservation.valid = 0;
}

static void increment_pc(cpu_t* cpu, uint32_t instruction) {
    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode; 
    if (!cpu->trap_taken && !cpu->pc_written)
        cpu->pc += 4;
}

static void dispatch_instruction(cpu_t* cpu, uint32_t instruction) {
    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode;

    switch (opcode) {
        case LUI:
        case AUIPC: {
            utype_t decoded = decodeU(instruction);
            executeU(cpu, decoded);
            break;
        }
        
        case JAL: {
            jtype_t decoded = decodeJ(instruction);
            executeJ(cpu, decoded);
            break;
        }

        case JALR:
        case LOAD:
        case OP_IMM:
        case SYSTEM:
        case MISC_MEM:
        case OP_IMM_32: {
            itype_t decoded = decodeI(instruction);
            executeI(cpu, decoded);
            break;
        }

        case BRANCH: {
            btype_t decoded = decodeB(instruction);
            executeB(cpu, decoded);
            break;
        }

        case STORE: {
            stype_t decoded = decodeS(instruction);
            executeS(cpu, decoded);
            break;
        }

        case OP:
        case OP_32: {
            rtype_t decoded = decodeR(instruction);
            executeR(cpu, decoded);
            break;
        }

        case AMO: {
            atype_t decoded = decodeA(instruction);
            executeA(cpu, decoded);
            break;
        }

        default: {
            // illegal instruction
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction);
        }
    }
}

static void cpu_check_interrupts(cpu_t* cpu) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];
    uint64_t mip = cpu->csrs[CSR_MIP];
    uint64_t mie = cpu->csrs[CSR_MIE];
    uint64_t mideleg = cpu->csrs[CSR_MIDELEG];
    
    uint64_t pending = mip & mie; // checks if an interrupt is pending and it is enabled
    if (!pending)
        return;

    static const uint8_t priority[] = {
        11, // MEI
        3,  // MSI
        7,  // MTI
        9,  // SEI
        1,  // SSI
        5,  // STI
    };

    for (uint8_t i = 0; i < sizeof(priority); ++i) {
        uint8_t cause = priority[i];

        if (!(pending & (1ULL << cause)))
            continue;

        uint8_t delegated = (mideleg >> cause) & 0x1;
        if (!delegated) {
            uint8_t mie_global = (mstatus >> 3) & 1;
            if (cpu->priviledge == M_MODE && !mie_global)
                continue;

            raise_interrupt(cpu, cause);
            return;
        }

        uint8_t sie_global = (mstatus >> 1) & 0x1;
        if (cpu->priviledge == M_MODE)
            continue;

        if (cpu->priviledge == S_MODE && !sie_global)
            continue;

        raise_interrupt(cpu, cause);
        return;
    }
}

cpu_t cpu_init() {
    cpu_t cpu = { 0 };

    csr_load(&cpu);
    
    return cpu;
}

void cpu_reset(cpu_t* cpu, uint64_t reset_addr) {
    cpu->pc = reset_addr;
    cpu->priviledge = M_MODE;
    cpu->trap_taken = 0;
    
    memset(cpu->regs, 0, sizeof(cpu->regs));
    csr_reset(cpu);
}

fetch_result_t cpu_fetch(cpu_t* cpu) {
    translation_result_t result = translate_address(cpu, cpu->pc, ACCESS_FETCH);
    fetch_result_t fetch_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, cpu->pc);
        return fetch_result;
    }
    
    fetch_result.success = 1;
    fetch_result.value = cpu->bus->read32(cpu->bus->ctx, result.physical_address);
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

void cpu_step(cpu_t* cpu) {
    cpu->trap_taken = 0;
    cpu->csrs[CSR_MCYCLE]++;

    fetch_result_t result = cpu_fetch(cpu);
    if (!result.success)
        return;

    uint32_t instruction = result.value;
    dispatch_instruction(cpu, instruction);
    
    increment_pc(cpu, instruction);
    
    if (!cpu->trap_taken)
        cpu->csrs[CSR_MINSTRET]++;
    
    cpu_check_interrupts(cpu);
    cpu->pc_written = 0;
}

void cpu_set_interrupt_pending(cpu_t* cpu, uint8_t cause, uint8_t pending) {
    if (pending)
        cpu->csrs[CSR_MIP] |= 1ULL << cause;
    else
        cpu->csrs[CSR_MIP] &= ~(1ULL << cause);
}

load_result_t cpu_load8(cpu_t* cpu, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 }; 

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }

    load_result.success = 1;
    load_result.value = cpu->bus->read8(cpu->bus->ctx, result.physical_address);
    return load_result;
}

load_result_t cpu_load16(cpu_t* cpu, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }
        
    load_result.success = 1;
    load_result.value = cpu->bus->read16(cpu->bus->ctx, result.physical_address);
    return load_result;
}

load_result_t cpu_load32(cpu_t* cpu, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }

    load_result.success = 1;
    load_result.value = cpu->bus->read32(cpu->bus->ctx, result.physical_address);
    return load_result;
}

load_result_t cpu_load64(cpu_t* cpu, uint64_t vaddr) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_LOAD);
    load_result_t load_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return load_result;
    }
    
    load_result.success = 1;
    load_result.value = cpu->bus->read64(cpu->bus->ctx, result.physical_address);
    return load_result;
}

store_result_t cpu_store8(cpu_t* cpu, uint64_t vaddr, uint8_t value) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }
    
    cpu_invalidate_reservation(cpu, result.physical_address, 1);
    cpu->bus->write8(cpu->bus->ctx, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

store_result_t cpu_store16(cpu_t* cpu, uint64_t vaddr, uint16_t value) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }

    cpu_invalidate_reservation(cpu, result.physical_address, 2);
    cpu->bus->write16(cpu->bus->ctx, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

store_result_t cpu_store32(cpu_t* cpu, uint64_t vaddr, uint32_t value) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }

    cpu_invalidate_reservation(cpu, result.physical_address, 4);
    cpu->bus->write32(cpu->bus->ctx, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}

store_result_t cpu_store64(cpu_t* cpu, uint64_t vaddr, uint64_t value) {
    translation_result_t result = translate_address(cpu, vaddr, ACCESS_STORE);
    store_result_t store_result = { 0 };

    if (result.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, result.result, vaddr);
        return store_result;
    }

    cpu_invalidate_reservation(cpu, result.physical_address, 8);
    cpu->bus->write64(cpu->bus->ctx, result.physical_address, value);
    store_result.success = 1;
    return store_result;
}
