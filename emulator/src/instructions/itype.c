#include "instructions/itype.h"
#include "instructions/dispatchers/op_imm.h"
#include "instructions/dispatchers/op_imm_32.h"
#include "instructions/dispatchers/load.h"
#include "instructions/dispatchers/jalr.h"
#include "instructions/dispatchers/system.h"
#include "instructions/dispatchers/misc_mem.h"
#include "cpu.h"
#include "exception.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>


void executeI(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    switch(opcode) {
        case OP_IMM: dispatch_op_imm(cpu, mem, instruction); break;
        case OP_IMM_32: dispatch_op_imm_32(cpu, mem, instruction); break;
        case LOAD: dispatch_load(cpu, mem, instruction); break;
        case JALR: dispatch_jalr(cpu, mem, instruction); break;
        case SYSTEM: dispatch_system(cpu, mem, instruction); break;
        case MISC_MEM: dispatch_misc_mem(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0); break;
    } 
}
