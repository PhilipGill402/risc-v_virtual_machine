#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <stdint.h>
#include "memory.h"

typedef struct cpu_t {
    uint64_t regs[32];
    uint64_t pc;
} cpu_t;

cpu_t cpu_init();
void cpu_reset(cpu_t* cpu);
uint32_t cpu_fetch(cpu_t* cpu, memory_t* mem);
void cpu_step(cpu_t* cpu, memory_t* mem);
void cpu_write_reg(cpu_t* cpu, uint8_t reg_num, uint64_t value);
uint64_t cpu_read_reg(cpu_t* cpu, uint8_t reg_num);

#endif
