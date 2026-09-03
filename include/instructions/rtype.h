#ifndef INCLUDE_INSTRUCTIONS_RTYPE_H_
#define INCLUDE_INSTRUCTIONS_RTYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

class CPU;

void executeR(CPU& cpu, Memory& mem, RType instruction);

#endif
