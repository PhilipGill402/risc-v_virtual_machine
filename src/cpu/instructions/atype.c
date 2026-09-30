#include "cpu/instructions/atype.h"
#include "cpu/instructions/dispatchers/a_extension.h"
#include "cpu/cpu.h"

void executeA(cpu_t* cpu, atype_t instruction) {
    dispatch_a_extension(cpu, instruction);
}
