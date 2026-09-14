#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <stdint.h>
#include "memory.h"
#include "csrs/csr.h"

typedef struct cpu_t {
    uint64_t regs[32];
    uint64_t csrs[4096];
    uint64_t pc;
    priviledge_t priviledge;

    uint8_t trap_taken;
} cpu_t;

cpu_t cpu_init();
void cpu_reset(cpu_t* cpu);
uint32_t cpu_fetch(cpu_t* cpu, memory_t* mem);
void cpu_step(cpu_t* cpu, memory_t* mem);
void cpu_write_reg(cpu_t* cpu, uint8_t reg_num, uint64_t value);
uint64_t cpu_read_reg(cpu_t* cpu, uint8_t reg_num);
void cpu_set_interrupt_pending(cpu_t* cpu, uint8_t cause, uint8_t pending);

#endif
