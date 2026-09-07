#include "tables.h"
#include <stddef.h>

const char* branch_mnemonics[8] = {
    "beq",     // 0b000
    "bne",     // 0b001
    NULL,      // 0b010 - invalid
    NULL,      // 0b011 - invalid
    "blt",     // 0b100
    "bge",     // 0b101
    "bltu",    // 0b110
    "bgeu"     // 0b111
};

const char* store_mnemonics[8] = {
    "sb",      // 0b000
    "sh",      // 0b001
    "sw",      // 0b010
    "sd",      // 0b011
    NULL,      // 0b100 - invalid
    NULL,      // 0b101 - invalid
    NULL,      // 0b110 - invalid
    NULL       // 0b111 - invalid
};

const char* op_mnemonics[8] = {
    "add",   // 000
    "sll",   // 001
    "slt",   // 010
    "sltu",  // 011
    "xor",   // 100
    "srl",   // 101
    "or",    // 110
    "and"    // 111
};

 const char* op_alt_mnemonics[8] = {
    "sub",   // 000
    NULL,
    NULL,
    NULL,
    NULL,
    "sra",   // 101
    NULL,
    NULL
};

 const char* op32_mnemonics[8] = {
    "addw",  // 000
    "sllw",  // 001
    NULL,    // 010
    NULL,    // 011
    NULL,    // 100
    "srlw",  // 101
    NULL,    // 110
    NULL     // 111
};

 const char* op32_alt_mnemonics[8] = {
    "subw",  // 000
    NULL,
    NULL,
    NULL,
    NULL,
    "sraw",  // 101
    NULL,
    NULL
};

 const char* load_mnemonics[8] = {
    "lb",   // 000
    "lh",   // 001
    "lw",   // 010
    "ld",   // 011
    "lbu",  // 100
    "lhu",  // 101
    "lwu",  // 110
    NULL    // 111
};

 const char* system_mnemonics[8] = {
    NULL,      // 000 - special SYSTEM instructions
    "csrrw",   // 001
    "csrrs",   // 010
    "csrrc",   // 011
    NULL,      // 100 - reserved
    "csrrwi",  // 101
    "csrrsi",  // 110
    "csrrci"   // 111
};

 const char* misc_mem_mnemonics[8] = {
    "fence",    // 000
    "fence.i",  // 001 - Zifencei
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

 const char* op_imm_32_mnemonics[8] = {
    "addiw",  // 000
    "slliw",  // 001
    NULL,     // 010
    NULL,     // 011
    NULL,     // 100
    "srliw",  // 101
    NULL,     // 110
    NULL      // 111
};

 const char* op_imm_32_alt_mnemonics[8] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "sraiw",  // 101
    NULL,
    NULL
};

 const char* op_imm_mnemonics[8] = {
    "addi",  // 000
    "slli",  // 001
    "slti",  // 010
    "sltiu", // 011
    "xori",  // 100
    "srli",  // 101
    "ori",   // 110
    "andi"   // 111
};

 const char* op_imm_alt_mnemonics[8] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "srai",  // 101
    NULL,
    NULL
};


