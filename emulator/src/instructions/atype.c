#include "instructions/atype.h"
#include "instructions/dispatchers/a_extension.h"
#include "cpu.h"

void executeA(cpu_t* cpu, atype_t instruction) {
    dispatch_a_extension(cpu, instruction);
}
