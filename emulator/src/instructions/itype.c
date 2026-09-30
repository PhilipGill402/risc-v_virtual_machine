#include "instructions/itype.h"
#include "instructions/dispatchers/op_imm.h"
#include "instructions/dispatchers/op_imm_32.h"
#include "instructions/dispatchers/load.h"
#include "instructions/dispatchers/jalr.h"
#include "instructions/dispatchers/system.h"
#include "instructions/dispatchers/misc_mem.h"
#include "cpu.h"
#include "trap.h"
#include "log.h"


void executeI(cpu_t* cpu, itype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    switch(opcode) {
        case OP_IMM: dispatch_op_imm(cpu, instruction); break;
        case OP_IMM_32: dispatch_op_imm_32(cpu, instruction); break;
        case LOAD: dispatch_load(cpu, instruction); break;
        case JALR: dispatch_jalr(cpu, instruction); break;
        case SYSTEM: dispatch_system(cpu, instruction); break;
        case MISC_MEM: dispatch_misc_mem(cpu, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw); break;
    } 
}
