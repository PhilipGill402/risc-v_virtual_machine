#include "cpu/instructions/jtype.h"
#include "cpu/instructions/dispatchers/jal.h"

void executeJ(cpu_t* cpu, jtype_t instruction) {
    dispatch_jal(cpu, instruction); 
}
