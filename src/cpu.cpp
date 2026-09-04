#include "cpu.h"
#include "instructions/decoding.h"
#include "instructions/itype.h"
#include "instructions/jtype.h"
#include "instructions/rtype.h"
#include "instructions/stype.h"
#include "instructions/utype.h"
#include "instructions/btype.h"

void CPU::reset() {
    pc = Memory::MEM_BASE;
}

uint32_t CPU::fetch(const Memory& mem) {
    return mem.read32(pc);
}

void CPU::write_reg(uint8_t reg_num, uint64_t value) {
    regs.write(reg_num, value);
}

uint64_t CPU::read_reg(uint8_t reg_num) const {
    return regs.read(reg_num);
}

void CPU::step(Memory& mem) {
    uint32_t instruction = fetch(mem);

    uint8_t raw_opcode = instruction & 0x7F;
    Opcode opcode = static_cast<Opcode>(raw_opcode);

    switch (opcode) {
        case Opcode::LUI:
        case Opcode::AUIPC: {
            UType decoded = decodeU(instruction);
            executeU(*this, mem, decoded);
            break;
        }
        
        case Opcode::JAL: {
            JType decoded = decodeJ(instruction);
            executeJ(*this, mem, decoded);
            break;
        }

        case Opcode::JALR:
        case Opcode::LOAD:
        case Opcode::OP_IMM:
        case Opcode::SYSTEM:
        case Opcode::MISC_MEM:
        case Opcode::OP_IMM_32: {
            IType decoded = decodeI(instruction);
            executeI(*this, mem, decoded);
            break;
        }

        case Opcode::BRANCH: {
            BType decoded = decodeB(instruction);
            executeB(*this, mem, decoded);
            break;
        }

        case Opcode::STORE: {
            SType decoded = decodeS(instruction);
            executeS(*this, mem, decoded);
            break;
        }

        case Opcode::OP:
        case Opcode::OP_32: {
            RType decoded = decodeR(instruction);
            executeR(*this, mem, decoded);
            break;
        }

        default:
            // illegal instruction
            break;
    }
}
