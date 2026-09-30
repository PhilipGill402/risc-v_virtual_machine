#include "instructions/stype.h"
#include "instructions/dispatchers/store.h"
#include "cpu.h"
#include "trap.h"

void executeS(cpu_t* cpu, stype_t instruction) {
    dispatch_store(cpu, instruction);
}
