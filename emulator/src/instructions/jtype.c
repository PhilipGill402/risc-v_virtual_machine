#include "instructions/jtype.h"
#include "instructions/dispatchers/jal.h"

void executeJ(cpu_t* cpu, memory_t* mem, jtype_t instruction) {
    dispatch_jal(cpu, mem, instruction); 
}
