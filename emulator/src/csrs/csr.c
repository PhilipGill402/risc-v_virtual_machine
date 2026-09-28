#include "csrs/csr.h"
#include "csrs/csr_def.h"
#include "csrs/mcsr.h"
#include "csrs/scsr.h"
#include "csrs/ucsr.h"
#include "cpu.h"

#include <stdio.h>

static uint8_t csr_can_write(uint16_t address, priviledge_t priviledge) {
    if ((uint8_t)((address >> 10) & 0x03) == CSR_READ_ONLY)
        return 0;
    
    if ((uint8_t)((address >> 8) & 0x03) > priviledge)
        return 0;

    return 1;
}

static uint8_t csr_can_read(cpu_t* cpu, uint16_t address, priviledge_t priviledge) {
    if ((uint8_t)((address >> 8) & 0x03) > priviledge)
        return 0;
    
    if (address == CSR_CYCLE || address == CSR_TIME || address == CSR_INSTRET) {
        uint8_t bit;
        if (address == CSR_CYCLE)
            bit = 0;
        else if (address == CSR_TIME)
            bit = 1;
        else
            bit = 2;

        uint8_t mcounteren_bit = cpu->csrs[CSR_MCOUNTEREN] & (1ULL << bit);
        uint8_t scounteren_bit = cpu->csrs[CSR_SCOUNTEREN] & (1ULL << bit);
        if (cpu->priviledge == S_MODE && !mcounteren_bit)
            return 0;
        else if (cpu->priviledge == U_MODE && (!mcounteren_bit || !scounteren_bit))
            return 0;
    } 
     

    return 1;
}

csr_status_t csr_read(cpu_t* cpu, uint16_t address, uint64_t* value) {
    if (address >= 4096)
        return CSR_ILLEGAL;

    csr_descriptor_t csr = csr_table[address];
    
    if (!csr.implemented)
        return CSR_ILLEGAL;
    
    if (!csr_can_read(cpu, address, cpu->priviledge))
        return CSR_ILLEGAL;
    
    if (csr.read)
        *value = csr.read(cpu);
    else
        *value = cpu->csrs[address];
    
    return CSR_OK;
}

csr_status_t csr_write(cpu_t* cpu, uint16_t address, uint64_t value) {
    if (address >= 4096)
        return CSR_ILLEGAL;

    csr_descriptor_t csr = csr_table[address];

    if (!csr.implemented)
        return CSR_ILLEGAL;

    if (!csr_can_write(address, cpu->priviledge))
        return CSR_ILLEGAL;

    if (csr.write) {
        csr.write(cpu, value);
    } else {
        uint64_t old = cpu->csrs[address];
        uint64_t mask = csr.write_mask;

        cpu->csrs[address] = (old & ~mask) | (value & mask);
    }

    return CSR_OK;
}

void csr_load(cpu_t* cpu) {
    mcsr_load_table(cpu);
    scsr_load_table(cpu);
    ucsr_load_table(cpu);
}

void csr_reset(cpu_t* cpu) {
    mcsr_reset(cpu);
    scsr_reset(cpu);
    ucsr_reset(cpu);
}


