#include "cpu/instructions/stype.h"
#include "cpu/instructions/dispatchers/store.h"
#include "cpu/cpu.h"
#include "cpu/trap.h"

void executeS(cpu_t* cpu, stype_t instruction) {
    dispatch_store(cpu, instruction);
}
