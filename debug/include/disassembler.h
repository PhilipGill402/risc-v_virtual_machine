#ifndef DEBUG_INCLUDE_CLI_DISASSEMBLER_H_
#define DEBUG_INCLUDE_CLI_DISASSEMBLER_H_

#include <stdint.h>
#include "cpu.h"

#define BUFFER_SIZE 256

char* disassemble_line(memory_t* mem, uint64_t addr);

#endif
