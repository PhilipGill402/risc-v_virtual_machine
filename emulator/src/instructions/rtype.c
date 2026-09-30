#include "instructions/rtype.h"
#include "instructions/dispatchers/m_extension.h"
#include "instructions/dispatchers/op.h"
#include "instructions/dispatchers/op_32.h"
#include "cpu.h"
#include "trap.h"

void executeR(cpu_t* cpu, rtype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    switch(opcode) {
        case OP: {
            // M Extension 
            if (instruction.funct7 == 0x01) {
                dispatch_m_op(cpu, instruction);
                break;
            }       

            dispatch_op(cpu, instruction);
            break;
        }
        case OP_32: {
            // M Extension 
            if (instruction.funct7 == 0x01) {
                dispatch_m_op_32(cpu, instruction);
                break;
            }

            dispatch_op_32(cpu, instruction);
            break;
        }
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw); break;
    } 
}



