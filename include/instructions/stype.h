#ifndef INCLUDE_INSTRUCTIONS_STYPE_H_
#define INCLUDE_INSTRUCTIONS_STYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

class CPU;

void executeS(CPU& cpu, Memory& mem, SType instruction);

#endif
