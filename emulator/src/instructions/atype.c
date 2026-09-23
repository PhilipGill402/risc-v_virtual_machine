#include "instructions/atype.h"
#include "instructions/dispatchers/a_extension.h"
#include "cpu.h"

void executeA(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    dispatch_a_extension(cpu, mem, instruction);
}
