#include "instructions/btype.h"
#include "instructions/dispatchers/branch.h"

void executeB(cpu_t* cpu, btype_t instruction) {
    dispatch_branch(cpu, instruction);
}
