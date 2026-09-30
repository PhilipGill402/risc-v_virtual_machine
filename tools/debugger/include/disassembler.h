#ifndef DEBUG_INCLUDE_CLI_DISASSEMBLER_H_
#define DEBUG_INCLUDE_CLI_DISASSEMBLER_H_

#include <stdint.h>
#include "vm/vm.h"

#define BUFFER_SIZE 256

char* disassemble_line(vm_t* vm, uint64_t addr);

#endif
