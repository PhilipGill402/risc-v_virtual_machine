#include "instructions/btype.h"
#include "instructions/dispatchers/branch.h"

void executeB(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    dispatch_branch(cpu, mem, instruction);
}
