#include "instructions/stype.h"
#include "instructions/dispatchers/store.h"
#include "cpu.h"
#include "exception.h"
#include <stdio.h>

void executeS(cpu_t* cpu, memory_t* mem, stype_t instruction) {
    dispatch_store(cpu, mem, instruction);
}
