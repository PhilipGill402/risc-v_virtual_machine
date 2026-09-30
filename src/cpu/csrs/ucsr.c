#include "cpu/csrs/ucsr.h"
#include "cpu/cpu.h"

static uint64_t cycle_read(cpu_t* cpu) {
    return cpu->csrs[CSR_MCYCLE];
}

static void ucsr_load_cycle(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_CYCLE];
    csr->implemented = 1;
    csr->read = cycle_read;
    csr->write_mask = 0; // read only
}

static uint64_t instret_read(cpu_t* cpu) {
    return cpu->csrs[CSR_MINSTRET];
}

static void ucsr_load_instret(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_INSTRET];
    csr->implemented = 1;
    csr->read = instret_read;
    csr->write_mask = 0; // read only
}

static uint64_t time_read(cpu_t* cpu) {
    return cpu->bus->read64(cpu->bus->ctx, 0x02000000 + 0xBFF8); // mtime offset
}

static void ucsr_load_time(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_TIME];
    csr->implemented = 1;
    csr->read = time_read;
    csr->write_mask = 0; // read only
}

void ucsr_load_table(cpu_t* cpu) {
    ucsr_load_time(cpu);
    ucsr_load_instret(cpu);
    ucsr_load_cycle(cpu);
}

void ucsr_reset(cpu_t* cpu) {
    // no need to set these csrs because they are just shadow registers 
    return;
}
