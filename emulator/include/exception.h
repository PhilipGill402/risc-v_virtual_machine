#ifndef EMULATOR_INCLUDE_EXCEPTION_H_
#define EMULATOR_INCLUDE_EXCEPTION_H_

#include <stdint.h>

#define EXC_INSTR_ADDR_MISALIGNED   0
#define EXC_INSTR_ACCESS_FAULT      1
#define EXC_ILLEGAL_INSTRUCTION     2
#define EXC_BREAKPOINT              3
#define EXC_LOAD_ADDR_MISALIGNED    4
#define EXC_LOAD_ACCESS_FAULT       5
#define EXC_STORE_ADDR_MISALIGNED   6
#define EXC_STORE_ACCESS_FAULT      7

#define EXC_ECALL_U_MODE            8
#define EXC_ECALL_S_MODE            9
#define EXC_ECALL_M_MODE            11

#define EXC_INSTR_PAGE_FAULT        12
#define EXC_LOAD_PAGE_FAULT         13
#define EXC_STORE_PAGE_FAULT        15

typedef struct cpu_t cpu_t;

void raise_exception(cpu_t* cpu, uint64_t cause, uint64_t tvalue); 

#endif
