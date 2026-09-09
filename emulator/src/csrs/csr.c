#include "csrs/csr.h"
#include "csrs/csr_def.h"
#include "cpu.h"

static uint8_t csr_can_write(uint16_t address, priviledge_t priviledge) {
    if ((uint8_t)((address >> 10) & 0x03) == CSR_READ_ONLY)
        return 0;
    
    if ((uint8_t)((address >> 8) & 0x03) > priviledge)
        return 0;

    return 1;
}

static uint8_t csr_can_read(uint16_t address, priviledge_t priviledge) {
    if ((uint8_t)((address >> 8) & 0x03) > priviledge)
        return 0;

    return 1;
}

csr_status_t csr_read(cpu_t* cpu, uint16_t address, uint64_t* value) {
    if (address >= 4096)
        return CSR_ILLEGAL;

    csr_descriptor_t csr = csr_table[address];

    if (!csr.implemented)
        return CSR_ILLEGAL;

    if (!csr_can_read(address, cpu->priviledge))
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
