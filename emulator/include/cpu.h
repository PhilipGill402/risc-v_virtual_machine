#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <stdint.h>
#include "memory.h"
#include "csrs/csr.h"
#include "bus_def.h"

typedef struct reservation {
    uint8_t valid;
    uint64_t phys_addr;
    uint8_t size;
} reservation_t;

typedef struct cpu_t {
    uint64_t regs[32];
    uint64_t csrs[4096];
    uint64_t pc;
    priviledge_t priviledge;

    bus_t* bus;
    
    reservation_t reservation;

    uint8_t trap_taken;
} cpu_t;

typedef struct load_result {
    uint8_t success;
    uint64_t value;
} load_result_t;

typedef load_result_t store_result_t;
typedef load_result_t fetch_result_t;

cpu_t cpu_init();
void cpu_reset(cpu_t* cpu);
void cpu_step(cpu_t* cpu, memory_t* mem);
void cpu_write_reg(cpu_t* cpu, uint8_t reg_num, uint64_t value);
uint64_t cpu_read_reg(cpu_t* cpu, uint8_t reg_num);
void cpu_set_interrupt_pending(cpu_t* cpu, uint8_t cause, uint8_t pending);
void cpu_invalidate_reservation(cpu_t* cpu, uint64_t phys_addr, uint8_t size);

load_result_t cpu_load8(cpu_t* cpu, memory_t* mem, uint64_t vaddr);
load_result_t cpu_load16(cpu_t* cpu, memory_t* mem, uint64_t vaddr);
load_result_t cpu_load32(cpu_t* cpu, memory_t* mem, uint64_t vaddr);
load_result_t cpu_load64(cpu_t* cpu, memory_t* mem, uint64_t vaddr);

store_result_t cpu_store8(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint8_t value);
store_result_t cpu_store16(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint16_t value);
store_result_t cpu_store32(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint32_t value);
store_result_t cpu_store64(cpu_t* cpu, memory_t* mem, uint64_t vaddr, uint64_t value);

#endif
