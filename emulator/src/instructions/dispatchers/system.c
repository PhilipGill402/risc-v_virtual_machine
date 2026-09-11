#include "instructions/dispatchers/system.h"
#include "csr_def.h"
#include "csr.h"
#include "cpu.h"
#include "exception.h"

static void ecall(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t cause = EXC_ECALL_U_MODE; 
    if (cpu->priviledge == M_MODE)
        cause = EXC_ECALL_M_MODE;
    else if (cpu->priviledge == S_MODE)
        cause = EXC_ECALL_S_MODE;
    else if (cpu->priviledge == U_MODE)
        cause = EXC_ECALL_U_MODE;

    raise_exception(cpu, cause, 0);
}

static void ebreak(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    raise_exception(cpu, EXC_BREAKPOINT, 0);    
}

static void sret(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];

    uint8_t spp = (uint8_t)(mstatus >> 8) & 0x1;
    uint8_t spie = (uint8_t)(mstatus >> 5) & 0x1;
    
    mstatus = set_bit(mstatus, 1, spie); // SIE = SPIE
    mstatus = set_bit(mstatus, 5, 1); // SPIE = 1
    
    // MPP = U mode
    mstatus = set_bit(mstatus, 8, U_MODE); // SPP = U_MODE
    
    cpu->priviledge = spp;
    cpu->csrs[CSR_MSTATUS] = mstatus;
    cpu->pc = cpu->csrs[CSR_SEPC];

    return;
}

static void mret(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];

    uint8_t mpp = (uint8_t)(mstatus >> 11) & 0x3;
    uint8_t mpie = (uint8_t)(mstatus >> 7) & 0x1;
    
    mstatus = set_bit(mstatus, 3, mpie); // MIE = MPIE
    mstatus = set_bit(mstatus, 7, 1); // MPIE = 1
    
    // MPP = M mode, change to user when U mode is implemented
    mstatus = set_bit(mstatus, 11, 1);
    mstatus = set_bit(mstatus, 12, 1);
    
    cpu->priviledge = mpp;
    cpu->csrs[CSR_MSTATUS] = mstatus;
    cpu->pc = cpu->csrs[CSR_MEPC];
}

static void wfi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    return;
}

static void csrrw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint16_t addr = (uint16_t)instruction.imm;
    uint64_t new = cpu_read_reg(cpu, instruction.rs1);
    uint64_t old = 0;

    if (instruction.rd != 0)
        csr_status_t status = csr_read(cpu, addr, &old);

    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    status = csr_write(cpu, addr, new);

    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    cpu_write_reg(cpu, instruction.rd, old);
}

static void csrrs(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint16_t addr = (uint16_t)instruction.imm;
   
    uint64_t old = 0;
    csr_status_t status = csr_read(cpu, addr, &old);
    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    if (instruction.rs1 != 0) {
        uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
        uint64_t new = old | rs1;
        
        status = csr_write(cpu, addr, new);
        if (status != CSR_OK) {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            return;
        }
    }
    
    cpu_write_reg(cpu, instruction.rd, old);
}

static void csrrc(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint16_t addr = (uint16_t)instruction.imm;
   
    uint64_t old = 0;
    csr_status_t status = csr_read(cpu, addr, &old);
    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    if (instruction.rs1 != 0) {
        uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
        uint64_t new = old & ~rs1;
        
        status = csr_write(cpu, addr, new);
        if (status != CSR_OK) {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            return;
        }
    }
    
    cpu_write_reg(cpu, instruction.rd, old);
}

static void csrrwi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint16_t addr = (uint16_t)instruction.imm;
    uint64_t old = 0;

    if (instruction.rd != 0) {
        csr_status_t status = csr_read(cpu, addr, &old);
        if (status != CSR_OK) {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            return;
        }
    }
    
    uint8_t zimm = instruction.rs1;
    csr_status_t status = csr_write(cpu, addr, zimm);
    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    if (instruction.rd != 0)
        cpu_write_reg(cpu, instruction.rd, old);
}

static void csrrsi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint16_t addr = (uint16_t)instruction.imm;
   
    uint64_t old = 0;
    csr_status_t status = csr_read(cpu, addr, &old);
    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    if (instruction.rs1 != 0) {
        uint8_t zimm = instruction.rs1;
        uint64_t new = old | zimm;
        
        status = csr_write(cpu, addr, new);
        if (status != CSR_OK) {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            return;
        }
    }
    
    cpu_write_reg(cpu, instruction.rd, old);   
}

static void csrrci(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint16_t addr = (uint16_t)instruction.imm;
   
    uint64_t old = 0;
    csr_status_t status = csr_read(cpu, addr, &old);
    if (status != CSR_OK) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    }

    if (instruction.rs1 != 0) {
        uint8_t zimm = instruction.rs1;
        uint64_t new = old & ~zimm;
        
        status = csr_write(cpu, addr, new);
        if (status != CSR_OK) {
            raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            return;
        }
    }
    
    cpu_write_reg(cpu, instruction.rd, old);
}

void dispatch_system(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: {
            switch (instruction.imm) {
                case 0x000: ecall(cpu, mem, instruction); break;
                case 0x001: ebreak(cpu, mem, instruction); break;
                case 0x102: sret(cpu, mem, instruction); break;
                case 0x302: mret(cpu, mem, instruction); break;
                case 0x105: wfi(cpu, mem, instruction); break;
                default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            }
            break;
        }
        case 0x1: csrrw(cpu, mem, instruction); break;
        case 0x2: csrrs(cpu, mem, instruction); break;
        case 0x3: csrrc(cpu, mem, instruction); break;
        case 0x5: csrrwi(cpu, mem, instruction); break;
        case 0x6: csrrsi(cpu, mem, instruction); break;
        case 0x7: csrrci(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
    }
}
