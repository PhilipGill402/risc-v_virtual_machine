#include "instructions/dispatchers/a_extension.h"
#include "cpu.h"
#include "trap.h"
#include "paging.h"

static void amo_w(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1);
    uint32_t rhs = (uint32_t)cpu_read_reg(cpu, instruction.rs2);
    
    if (addr & 0x3) {
        raise_exception(cpu, EXC_STORE_ADDR_MISALIGNED, addr);
        return;
    }
    
    translation_result_t tr = translate_address(cpu, mem, addr, ACCESS_STORE);
    if (tr.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, tr.result, addr);
        return;
    }

    uint32_t old = cpu->bus->read32(cpu->bus->ctx, tr.physical_address);
    uint32_t new;

    switch (instruction.funct5) {
        case AMOSWAP: {
            new = rhs;
            break;
        }

        case AMOADD: {
            new = old + rhs;
            break;
        }

        case AMOXOR: {
            new = old ^ rhs;
            break;
        }

        case AMOAND: {
            new = old & rhs;
            break;
        }

        case AMOOR: {
            new = old | rhs;
            break;
        }

        case AMOMIN: {
            new = (int32_t)old < (int32_t)rhs ? old : rhs;
            break;
        }
        
        case AMOMAX: {
            new = (int32_t)old > (int32_t)rhs ? old : rhs;
            break;
        }

        case AMOMINU: {
            new = old < rhs ? old : rhs;
            break;
        }

        case AMOMAXU: {
            new = old > rhs ? old : rhs;
            break;
        }

        default: {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
            return;
        }
    }
    
    cpu_invalidate_reservation(cpu, tr.physical_address, 4);
    cpu->bus->write32(cpu->bus->ctx, tr.physical_address, new); 
    cpu_write_reg(cpu, instruction.rd, sign_extend(old, 32));
}

static void amo_d(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rhs = cpu_read_reg(cpu, instruction.rs2);
    
    if (addr & 0x7) {
        raise_exception(cpu, EXC_STORE_ADDR_MISALIGNED, addr);
        return;
    }
    
    translation_result_t tr = translate_address(cpu, mem, addr, ACCESS_STORE);
    if (tr.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, tr.result, addr);
        return;
    }

    uint64_t old = cpu->bus->read64(cpu->bus->ctx, tr.physical_address);
    uint64_t new;

    switch (instruction.funct5) {
        case AMOSWAP: {
            new = rhs;
            break;
        }

        case AMOADD: {
            new = old + rhs;
            break;
        }

        case AMOXOR: {
            new = old ^ rhs;
            break;
        }

        case AMOAND: {
            new = old & rhs;
            break;
        }

        case AMOOR: {
            new = old | rhs;
            break;
        }

        case AMOMIN: {
            new = (int64_t)old < (int64_t)rhs ? old : rhs;
            break;
        }
        
        case AMOMAX: {
            new = (int64_t)old > (int64_t)rhs ? old : rhs;
            break;
        }

        case AMOMINU: {
            new = old < rhs ? old : rhs;
            break;
        }

        case AMOMAXU: {
            new = old > rhs ? old : rhs;
            break;
        }

        default: {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
            return;
        }
    }

    cpu_invalidate_reservation(cpu, tr.physical_address, 8);
    cpu->bus->write64(cpu->bus->ctx, tr.physical_address, new);
    cpu_write_reg(cpu, instruction.rd, old);
}

static void lr_d(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    if (instruction.rs2 != 0) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
        return;
    }

    uint64_t addr = cpu_read_reg(cpu, instruction.rs1);

    if (addr & 0x7) {
        raise_exception(cpu, EXC_LOAD_ADDR_MISALIGNED, addr);
        return;
    }

    translation_result_t tr = translate_address(cpu, mem, addr, ACCESS_LOAD);
    if (tr.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, tr.result, addr);
        return;
    }

    uint64_t value = cpu->bus->read64(cpu->bus->ctx, tr.physical_address);
    cpu_write_reg(cpu, instruction.rd, value);

    cpu->reservation.valid = 1;
    cpu->reservation.phys_addr = tr.physical_address;
    cpu->reservation.size = 8;
}

static void lr_w(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    if (instruction.rs2 != 0) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
        return;
    }

    uint64_t addr = cpu_read_reg(cpu, instruction.rs1);

    if (addr & 0x3) {
        raise_exception(cpu, EXC_LOAD_ADDR_MISALIGNED, addr);
        return;
    }

    translation_result_t tr = translate_address(cpu, mem, addr, ACCESS_LOAD);
    if (tr.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, tr.result, addr);
        return;
    }

    uint32_t value = cpu->bus->read32(cpu->bus->ctx, tr.physical_address);
    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 32));

    cpu->reservation.valid = 1;
    cpu->reservation.phys_addr = tr.physical_address;
    cpu->reservation.size = 4;
}

static void sc_d(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1);
    if (addr & 0x7) {
        raise_exception(cpu, EXC_STORE_ADDR_MISALIGNED, addr);
        return;
    }

    uint64_t value = cpu_read_reg(cpu, instruction.rs2);

    translation_result_t tr = translate_address(cpu, mem, addr, ACCESS_STORE);
    if (tr.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, tr.result, addr);
        return;
    }

    if (cpu->reservation.phys_addr == tr.physical_address && cpu->reservation.valid && cpu->reservation.size == 8) {
        cpu->bus->write64(cpu->bus->ctx, tr.physical_address, value);
        cpu_write_reg(cpu, instruction.rd, 0);
    } else {
        cpu_write_reg(cpu, instruction.rd, 1);
    }

    cpu->reservation.valid = 0;
}

static void sc_w(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1);
    if (addr & 0x3) {
        raise_exception(cpu, EXC_STORE_ADDR_MISALIGNED, addr);
        return;
    }

    uint32_t value = (uint32_t)cpu_read_reg(cpu, instruction.rs2);

    translation_result_t tr = translate_address(cpu, mem, addr, ACCESS_STORE);
    if (tr.result != TRANSLATION_SUCCESS) {
        raise_exception(cpu, tr.result, addr);
        return;
    }

    if (cpu->reservation.phys_addr == tr.physical_address && cpu->reservation.valid && cpu->reservation.size == 4) {
        cpu->bus->write32(cpu->bus->ctx, tr.physical_address, value);
        cpu_write_reg(cpu, instruction.rd, 0);
    } else {
        cpu_write_reg(cpu, instruction.rd, 1);
    }

    cpu->reservation.valid = 0;
}

void dispatch_a_extension(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    switch (instruction.funct5) {
        case AMOADD:
        case AMOSWAP:
        case AMOXOR:
        case AMOOR:
        case AMOAND:
        case AMOMIN:
        case AMOMAX:
        case AMOMINU:
        case AMOMAXU: {
            if (instruction.funct3 == 0x2)
                amo_w(cpu, mem, instruction);
            else if (instruction.funct3 == 0x3)
                amo_d(cpu, mem, instruction);
            else
                raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
            break;
        }

        case LR: {
            if (instruction.funct3 == 0x2) 
                lr_w(cpu, mem, instruction);
            else if (instruction.funct3 == 0x3)
                lr_d(cpu, mem, instruction);
            else
                raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
            break;
        }

        case SC: {
            if (instruction.funct3 == 0x2) 
                sc_w(cpu, mem, instruction);
            else if (instruction.funct3 == 0x3)
                sc_d(cpu, mem, instruction);
            else
                raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
            break;
        }

        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
    }
}
