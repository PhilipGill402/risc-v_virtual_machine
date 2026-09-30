#include "instructions/jtype.h"
#include "instructions/dispatchers/jal.h"

void executeJ(cpu_t* cpu, jtype_t instruction) {
    dispatch_jal(cpu, instruction); 
}
