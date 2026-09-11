#include "instructions/dispatchers/misc_mem.h"
#include "log.h"

static void fence(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    log_error("FENCE not implemented\n");
}

void dispatch_misc_mem(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    fence(cpu, mem, instruction);
}
